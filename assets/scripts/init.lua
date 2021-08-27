SetSkybox("assets/images/GCanyon_C_YumaPoint_8k.jpg")

local backpack = AddEntity()
AddTransform(backpack, 0, 0, 0, 0, 0, 0, 1, 1, 1)
AddMeshRenderer(backpack, "assets/models/backpack/backpack.obj", false)

local bob = AddEntity()
AddTransform(bob, 5, 0, 0, 0, 0, 0, 0.05, 0.05, 0.05)
AddMeshRenderer(bob, "assets/models/boblampclean/boblampclean.md5mesh", true)

local dude = AddEntity()
AddTransform(dude, -5, 0, 0, 0, 0, 0, 1, 1, 1)
AddMeshRenderer(dude, "assets/models/dude/model.dae", true)

local spinning_lights = AddEntity()
AddScript(spinning_lights, "assets/scripts/spinning_lights.lua")
