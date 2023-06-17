local flashlight
local flashlight_on = true
local flashlight_follow = true

function Lerp(a, b, x)
    return a + (b - a) * x
end

function Init()
    flashlight = GetEntity()
end

function Update(delta_time)
    if (GetKeyDown(9)) then
        flashlight_on = not flashlight_on
    end
    if (GetKeyDown(10)) then
        flashlight_follow = not flashlight_follow
    end

    if (flashlight_on) then
    
    else

    end

    if (flashlight_follow) then
        player = GetEntityByName("Player")
        if (player ~= -1) then
            player_x = GetTransformX(player)
            player_y = GetTransformY(player)
            player_z = GetTransformZ(player)
            flashlight_rot_x = GetTransformRotX(flashlight)
            flashlight_rot_y = GetTransformRotY(flashlight)
            flashlight_rot_z = GetTransformRotZ(flashlight)
            player_front_x = GetCameraFrontX(player)
            player_front_y = GetCameraFrontY(player)
            player_front_z = GetCameraFrontZ(player)
            rot_x = Lerp(flashlight_rot_x, player_front_x, 30 * delta_time)
            rot_y = Lerp(flashlight_rot_y, player_front_y, 30 * delta_time)
            rot_z = Lerp(flashlight_rot_z, player_front_z, 30 * delta_time)
            UpdateTransform(flashlight, player_x, player_y, player_z, rot_x, rot_y, rot_z, 1, 1, 1)
        end
    end
end
