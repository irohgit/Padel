-- BuildPlayerAssets.lua
-- Overview: builds each player's sprite sheet (PNG + JSON) and generates PlayerFrames.h,
-- the C++ frame and clip tables the game includes.
--
-- AUTHORSHIP
-- Design (Iroh Nguyen):
--     [1] Frame tables as plain SDL_Rect arrays, where the position is the index.
--     [2] Per-character clip tables of { Name, From, To }, with To exclusive.
--     [3] The shared EPlayerClip enum, in PascalCase.
--     [4] One generated header holding every character's tables.
--     [5] The Clip type kept hand-written in AnimationTypes.h (implemented by Iroh),
--         so the generated file holds data only.
--     [6] Generator logic, written as pseudocode: what is read from the JSON
--         ("frame", frameTags), what is written to disk, and in what order.
--     [7] File Header comment.
-- Lua implementation (Claude, Anthropic):
--     [1] Translated the pseudocode into Lua (LoadSheetData, ToPascalCase, WriteHeader).
--     [2] The sheet-export steps (1 to 7).
--     [3] Added no logic beyond a clip-name consistency check, which Iroh approved.
--
-- Aseprite script: for each character, combines every animation's PNG
-- sequence into one sprite (one tag per animation), trims the empty
-- border using the bounds of ALL frames, saves the .aseprite source,
-- and exports a sprite sheet (one row per tag) + JSON data.
--
-- Run it in Aseprite: File > Scripts > Open Scripts Folder, put this
-- file there, then File > Scripts > Rescan Scripts Folder, then run
-- File > Scripts > BuildPlayerAssets 

-- ===== Settings ============================================================
local ROOT        = "E:/cplusplus/Padel"
local FRAMES_DIR  = ROOT .. "/Assets/Textures/Players"   -- the 9 animation folders
local SOURCE_DIR  = ROOT .. "/Ignore_Files/Sheets"       -- .aseprite working files
local SHEET_DIR   = ROOT .. "/Assets/Textures/Players"   -- exported PNG + JSON

local CHARACTERS  = { "Alex", "Remi" }

-- Order here = tag order = row order in the exported sheet
local ANIMATIONS  = {
    "Idle",
    "Idle Blinking",
    "Moving Forward",
    "Moving Backward",
    "Hitting",
    "Hitting in The Air",
    "Jump Start",
    "Jump Loop",
    "Falling Down",
}

local GENERATED_HEADER = ROOT .. "/PlayerFrames.h"           -- frame + clip tables for the game

local FRAME_DURATION = 0.1   -- seconds per frame (Aseprite's default); tune per tag later
local TRIM           = true  -- Sprite > Trim using the bounds of every frame
local SCALE          = 0.5   -- e.g. 0.5 halves the art; 1.0 keeps it as is

-- ===== Helpers =============================================================

-- Sorted list of this character's frames in one animation folder
local function FramesFor(character, animation)
    local dir = app.fs.joinPath(FRAMES_DIR, animation)
    local prefix = character .. "_"
    local files = {}
    for _, name in ipairs(app.fs.listFiles(dir)) do
        if name:sub(1, #prefix) == prefix and name:lower():match("%.png$") then
            table.insert(files, app.fs.joinPath(dir, name))
        end
    end
    table.sort(files)   -- _000, _001, ... are zero-padded, so text order = frame order
    return files
end

-- Read an exported sprite-sheet JSON file and parse it into Lua tables.
-- Returns the parsed data, or nil and a reason if something failed.
local function LoadSheetData(jsonPath)
    if json == nil then
        return nil, "this Aseprite has no json module (needs Aseprite 1.3 or later)"
    end

    local file = io.open(jsonPath, "r")
    if not file then
        return nil, "could not open " .. jsonPath
    end
    local text = file:read("a")   -- the whole file as one string
    file:close()

    return json.decode(text)      -- JSON text -> nested Lua tables
end

-- "Hitting in The Air" -> "HittingInTheAir": remove spaces, capitalise every word.
-- C++ names can't contain spaces, so the enum uses this form.
local function ToPascalCase(name)
    local result = ""
    for word in name:gmatch("%S+") do                 -- each run of non-space characters
        result = result .. word:sub(1, 1):upper() .. word:sub(2)
    end
    return result
end

-- Write PlayerFrames.h from every character's parsed JSON.
-- sheets = { { name = "Alex", data = <parsed json> }, ... }
local function WriteHeader(sheets)
    -- Every character must have built, or the header would be missing tables.
    if #sheets ~= #CHARACTERS then
        print("ERROR: not every sheet was built, so " .. GENERATED_HEADER .. " was not written")
        return
    end

    -- The enum is shared, so every character needs the same clip names in the same order.
    local referenceTags = sheets[1].data.meta.frameTags
    for s = 2, #sheets do
        local tags = sheets[s].data.meta.frameTags
        if #tags ~= #referenceTags then
            print(string.format("ERROR: %s has %d clips but %s has %d; header not written",
                  sheets[s].name, #tags, sheets[1].name, #referenceTags))
            return
        end
        for i = 1, #referenceTags do
            if tags[i].name ~= referenceTags[i].name then
                print(string.format("ERROR: clip %d is '%s' for %s but '%s' for %s; header not written",
                      i - 1, tags[i].name, sheets[s].name, referenceTags[i].name, sheets[1].name))
                return
            end
        end
    end

    local out = io.open(GENERATED_HEADER, "w")         -- "w" creates the file, or empties it
    if not out then
        print("ERROR: could not open " .. GENERATED_HEADER .. " for writing")
        return
    end

    -- Top of the file
    out:write("// PlayerFrames.h: GENERATED by BuildPlayerAssets.lua from the Aseprite JSON.\n")
    out:write("// Do not edit by hand. Re-run the script to regenerate.\n")
    out:write("#pragma once\n")
    out:write("#include <SDL/SDL_rect.h>\n")
    out:write("#include \"AnimationTypes.h\"\n\n")

    -- Shared enum, in frameTags order: Idle = 0, IdleBlinking = 1, ...
    out:write("enum class EPlayerClip\n{\n")
    for i, tag in ipairs(referenceTags) do
        out:write(string.format("    %s,    // %d\n", ToPascalCase(tag.name), i - 1))
    end
    out:write("};\n\n")

    -- One frame table and one clip table per character
    for _, sheet in ipairs(sheets) do
        local upper  = sheet.name:upper()             -- "Alex" -> "ALEX"
        local frames = sheet.data.frames
        local tags   = sheet.data.meta.frameTags

        out:write(string.format("// ===== %s =====\n", sheet.name))

        -- Frame table: position = index, so no index is stored
        out:write(string.format("inline constexpr SDL_Rect %s_FRAMES[] =\n{\n", upper))
        for i, f in ipairs(frames) do
            local r = f.frame                         -- where the frame sits on the sheet
            out:write(string.format("    { %4d, %4d, %3d, %3d },   // %d\n", r.x, r.y, r.w, r.h, i - 1))
        end
        out:write("};\n")
        out:write(string.format("inline constexpr int %s_FRAME_COUNT = %d;\n\n", upper, #frames))

        -- Clip table: To is exclusive, so Aseprite's inclusive `to` gets + 1
        out:write(string.format("inline constexpr Clip %s_CLIPS[] =\n{\n", upper))
        for _, tag in ipairs(tags) do
            out:write(string.format("    { \"%s\", %d, %d },\n", tag.name, tag.from, tag.to + 1))
        end
        out:write("};\n")
        out:write(string.format("inline constexpr int %s_CLIP_COUNT = %d;\n\n", upper, #tags))
    end

    out:close()
    print("Wrote " .. GENERATED_HEADER)
end

local function BuildCharacter(character)
    -- 1. Read the first frame to learn the canvas size
    local first = FramesFor(character, ANIMATIONS[1])[1]
    if not first then
        print("No frames found for " .. character .. ", skipping")
        return
    end
    local probe = Image{ fromFile = first }
    local W, H = probe.width, probe.height

    -- 2. Empty RGB sprite; it starts with one empty frame
    local spr = Sprite(W, H, ColorMode.RGB)
    local layer = spr.layers[1]
    layer.name = character
    local frameCount = 0

    -- 3. Append every animation's frames, remembering each range.
    --    Tags are created AFTER all frames exist: adding frames at the end
    --    of an existing tag stretches that tag in Aseprite.
    local ranges = {}
    for _, animation in ipairs(ANIMATIONS) do
        local files = FramesFor(character, animation)
        if #files == 0 then
            print("  WARNING: no frames for " .. character .. " / " .. animation)
        else
            local fromFrame = frameCount + 1
            for _, path in ipairs(files) do
                local img = Image{ fromFile = path }
                if img.width ~= W or img.height ~= H then
                    error(path .. " is " .. img.width .. "x" .. img.height ..
                          ", expected " .. W .. "x" .. H)
                end
                frameCount = frameCount + 1
                if frameCount > #spr.frames then
                    spr:newEmptyFrame(frameCount)
                end
                spr:newCel(layer, frameCount, img, Point(0, 0))
                spr.frames[frameCount].duration = FRAME_DURATION
            end
            table.insert(ranges, { name = animation, from = fromFrame, to = frameCount })
        end
    end

    -- 3b. Now that every frame exists, tag each range
    for _, r in ipairs(ranges) do
        local tag = spr:newTag(r.from, r.to)
        tag.name = r.name
        print(string.format("  %-20s frames %3d-%3d (%d)",
              r.name, r.from, r.to, r.to - r.from + 1))
    end

    -- 4. Trim the empty border, using the union of all frames
    if TRIM then
        app.command.AutocropSprite()   -- Sprite > Trim
    end

    -- 5. Optional uniform scale
    if SCALE ~= 1.0 then
        spr:resize(math.floor(spr.width * SCALE + 0.5),
                   math.floor(spr.height * SCALE + 0.5))
    end

    -- 6. Save the working source file
    app.fs.makeAllDirectories(SOURCE_DIR)
    spr:saveAs(app.fs.joinPath(SOURCE_DIR, character .. ".aseprite"))

    -- 7. Export: one row per tag, PNG + JSON (array, with tags)
    app.command.ExportSpriteSheet{
        ui              = false,
        askOverwrite    = false,
        type            = SpriteSheetType.ROWS,
        splitTags       = true,
        textureFilename = app.fs.joinPath(SHEET_DIR, character .. ".png"),
        dataFilename    = app.fs.joinPath(SHEET_DIR, character .. ".json"),
        dataFormat      = SpriteSheetDataFormat.JSON_ARRAY,
        listTags        = true,
        openGenerated   = false,
    }

    -- 8. Parse the JSON we just exported
    local data, err = LoadSheetData(app.fs.joinPath(SHEET_DIR, character .. ".json"))
    if not data then
        print("  ERROR: " .. err)
    else
        print(string.format("  parsed %s: %d frames, %d tags",
              character, #data.frames, #data.meta.frameTags))
    end

    print(string.format("%s: %d frames, %d tags, frame size %dx%d (was %dx%d)",
          character, frameCount, #spr.tags, spr.width, spr.height, W, H))
    spr:close()

    return data   -- the parsed JSON (or nil if parsing failed), for WriteHeader
end

-- ===== Run =================================================================
local sheets = {}
for _, character in ipairs(CHARACTERS) do
    print("Building " .. character)
    local data = BuildCharacter(character)
    if data then
        table.insert(sheets, { name = character, data = data })
    end
end
print("Done. Sheets are in " .. SHEET_DIR)

WriteHeader(sheets)
