local flashlight

function Init()
    flashlight = GetEntity()
end

function Update(delta_time)
    -- UpdateTransform(flashlight, camera.position.x, camera.position.y, camera.position.z, camera.front.x, camera.front.y, camera.front.z, 1, 1, 1)
    -- registry.get<liminal::transform>(flashlight).rotation = glm::mix(
    --     registry.get<liminal::transform>(flashlight).rotation,
    --     camera_front,
    --     30.0f * delta_time);
end
