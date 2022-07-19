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
        camera_x = GetCameraX()
        camera_y = GetCameraY()
        camera_z = GetCameraZ()
        flashlight_rot_x = GetTransformRotX(flashlight)
        flashlight_rot_y = GetTransformRotY(flashlight)
        flashlight_rot_z = GetTransformRotZ(flashlight)
        camera_rot_x = GetCameraRotX()
        camera_rot_y = GetCameraRotY()
        camera_rot_z = GetCameraRotZ()
        rot_y = Lerp(flashlight_rot_y, camera_rot_y, 30 * delta_time)
        rot_z = Lerp(flashlight_rot_z, camera_rot_z, 30 * delta_time)
        rot_x = Lerp(flashlight_rot_x, camera_rot_x, 30 * delta_time)
        UpdateTransform(flashlight, camera_x, camera_y, camera_z, rot_x, rot_y, rot_z, 1, 1, 1)
    end
end
