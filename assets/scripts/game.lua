local backpack
local bob
local dude

local red_light
local yellow_light
local green_light
local blue_light
local light_distance = 5
local light_angle = 0

function Init()
    SetSkybox("assets/images/GCanyon_C_YumaPoint_8k.jpg")

    backpack = AddEntity()
    AddTransform(backpack, 0, 0, 0, 0, 0, 0, 1, 1, 1)
    AddMeshRenderer(backpack, "assets/models/backpack/backpack.obj", false)

    bob = AddEntity()
    AddTransform(bob, 5, 0, 0, 0, 0, 0, 0.05, 0.05, 0.05)
    AddMeshRenderer(bob, "assets/models/boblampclean/boblampclean.md5mesh", true)

    dude = AddEntity()
    AddTransform(dude, -5, 0, 0, 0, 0, 0, 1, 1, 1)
    AddMeshRenderer(dude, "assets/models/dude/model.dae", true)

    red_light = AddEntity()
    AddTransform(red_light, light_distance, 0, light_distance, 0, 0, 0, 1, 1, 1)
    AddPointLight(red_light, 10, 0, 0)

    yellow_light = AddEntity()
    AddTransform(yellow_light, -light_distance, 0, -light_distance, 0, 0, 0, 1,
                 1, 1)
    AddPointLight(yellow_light, 10, 10, 0)

    green_light = AddEntity()
    AddTransform(green_light, light_distance, 0, -light_distance, 0, 0, 0, 1, 1,
                 1)
    AddPointLight(green_light, 0, 10, 0)

    blue_light = AddEntity()
    AddTransform(blue_light, -light_distance, 0, light_distance, 0, 0, 0, 1, 1,
                 1)
    AddPointLight(blue_light, 0, 0, 10)
end

function Update(delta_time)
    light_angle = light_angle + 0.5 * delta_time;
    if (light_angle > 2 * math.pi) then light_angle = 0 end
    UpdateTransform(red_light, light_distance * math.sin(light_angle), 0,
                    light_distance * math.cos(light_angle), 0, 0, 0, 1, 1, 1)
    UpdateTransform(yellow_light,
                    light_distance * math.sin(light_angle + math.pi / 2), 0,
                    light_distance * math.cos(light_angle + math.pi / 2), 0, 0,
                    0, 1, 1, 1)
    UpdateTransform(green_light,
                    light_distance * math.sin(light_angle + math.pi), 0,
                    light_distance * math.cos(light_angle + math.pi), 0, 0, 0,
                    1, 1, 1)
    UpdateTransform(blue_light,
                    light_distance * math.sin(light_angle + 3 * math.pi / 2), 0,
                    light_distance * math.cos(light_angle + 3 * math.pi / 2), 0,
                    0, 0, 1, 1, 1)
end
