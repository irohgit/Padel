/*
 * PadelValidate.js
 * Content test for .PADEL (FMOD Studio 2.03 scripting API)
 *
 * Why:
 *   FMOD content has no compiler. A wrong file in a playlist builds and
 *   plays without error. I nearly dragged impactWood_medium into the
 *   Wall event, so content needs tests too.
 *
 * Contract (naming convention):
 *   Event name gives the material: Hit = impactWood, Wall = impactGlass
 *   Track name gives the weight:   Light / Medium / Heavy = _light_ / _medium_ / _heavy_
 *
 * What it checks (prototype, one case):
 *   Every take in event:/SFX/Wall, track Medium, starts with "impactGlass_medium_".
 *   Fails if any take is wrong, or if zero takes were checked
 *   (so a renamed track cannot pass silently).
 *
 * Usage:
 *   Save in PadelAudio/Scripts/, then Scripts > Reload.
 *   Run Scripts > Padel > Validate Wall Medium. The result shows in a popup
 *   and in Window > Console.
 *
 * Next step:
 *   A table of event, track and prefix covering all six playlists.
 *
 * Author: Iroh Nguyen, 27 Oct 2026
 * Prototype generated with Claude from my spec; run, verified (pass and fail)
 * and committed by me.
 */
// PadelValidate.js: content test for .PADEL
// Asserts one playlist holds the right takes: Wall / Medium = impactGlass_medium_*
studio.menu.addMenuItem({
    name: "Padel\\Validate Wall Medium",
    execute: function () {
        var event = studio.project.lookup("event:/SFX/Wall");
        if (!event) { console.log("FAIL: event:/SFX/Wall not found"); return; }

        var expected = "impactGlass_medium_";
        var checked = 0;
        var failed = 0;

        event.groupTracks.forEach(function (track) {
            if (track.mixerGroup.name !== "Medium") return;      // only the Medium track
            track.modules.forEach(function (instrument) {
                if (!instrument.sounds) return;                    // only multi instruments
                instrument.sounds.forEach(function (sound) {
                    var path = sound.audioFile.assetPath;          // e.g. SFX/Wall/impactGlass_medium_000.ogg
                    var file = path.substring(path.lastIndexOf("/") + 1);
                    checked++;
                    if (file.indexOf(expected) !== 0) {
                        failed++;
                        console.log("FAIL: Wall/Medium contains " + file);
                    }
                });
            });
        });

        // checked == 0 also fails: a renamed track must not pass silently
        var pass = (failed === 0 && checked > 0);
        console.log((pass ? "PASS" : "FAIL") + ": Wall/Medium, " + checked + " takes checked, " + failed + " wrong");
        studio.system.message((pass ? "PASS" : "FAIL") + ": Wall/Medium, " + checked + " takes checked, " + failed + " wrong");
    }
});
