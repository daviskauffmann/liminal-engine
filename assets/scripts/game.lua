local backpack
local backpack_transform

local bob
local dude

function Init()
    backpack = AddEntity()
    backpack_transform = AddTransform(backpack, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0)
    print(backpack_transform)
    AddMeshRenderer(backpack, "assets/models/backpack/backpack.obj", false)

    bob = AddEntity()
    AddTransform(bob, 5.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.05, 0.05, 0.05)
    AddMeshRenderer(bob, "assets/models/boblampclean/boblampclean.md5mesh", true)

    dude = AddEntity()
    AddTransform(dude, -5.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0)
    AddMeshRenderer(dude, "assets/models/dude/model.dae", true)
end

function Update(deltaTime) end
