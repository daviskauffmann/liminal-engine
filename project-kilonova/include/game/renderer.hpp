#ifndef GAME_RENDERER_HPP
#define GAME_RENDERER_HPP

enum RenderMode
{
    RENDER_MODE_FORWARD,
    RENDER_MODE_DEFERRED
};

class Renderer
{
public:
    Renderer(int renderWidth, int renderHeight, float renderScale, int shadowWidth, int shadowHeight);
    ~Renderer();

    void setMode(enum render_mode renderMode);
    void addObject(struct object *object);
    void setSun(struct sun *sun);
    void addDirectionalLight(struct directional_light *directionalLight);
    void addPointLight(struct point_light *pointLight);
    void addspotLight(struct spot_light *spotLight);
    void setSkybox(struct cubemap *skybox);
    void addWater(struct water *water);
    void addSprite(struct sprite *sprite);
    void draw(struct camera *camera, float aspect, unsigned int elapsedTime, float deltaTime) const;

private:
    // settings
    int renderWidth;
    int renderHeight;
    float renderScale;
    int shadowWidth;
    int shadowHeight;
    int reflectionWidth;
    int reflectionHeight;
    int refractionWidth;
    int refractionHeight;
    render_mode renderMode;

    // shader programs
    program *depthProgram;
    program *forwardColorProgram;
    program *forwardSunProgram;
    program *forwardDirectionalProgram;
    program *forwardPointProgram;
    program *forwardSpotProgram;
    program *forwardReflectionProgram;
    program *geometryProgram;
    program *deferredSunProgram;
    program *deferredDirectionalProgram;
    program *deferredPointProgram;
    program *deferredSpotProgram;
    program *skyboxProgram;
    program *waterProgram;
    program *spriteProgram;
    program *screenProgram;

    // framebuffers
    GLuint screenFboId;
    GLuint screenTextureId;
    GLuint screenRboId;

    GLuint depthmapFboId;
    GLuint depthmapTextureId;

    GLuint geometryFboId;
    GLuint geometryPositionTextureId;
    GLuint geometryNormalTextureId;
    GLuint geometryAlbedoSpecularTextureId;
    GLuint geometryRboId;

    GLuint waterReflectionFboId;
    GLuint waterReflectionColorTextureId;
    GLuint waterReflectionDepthTextureId;
    GLuint waterReflectionDepthRboId;

    GLuint waterRefractionFboId;
    GLuint waterRefractionColorTextureId;
    GLuint waterRefractionDepthTextureId;
    GLuint waterRefractionDepthRboId;

    // water mesh
    unsigned int numWaterVertices;
    GLuint waterVaoId;
    GLuint waterVboId;

    // skybox mesh
    unsigned int numSkyboxVertices;
    GLuint skyboxVaoId;
    GLuint skyboxVboId;

    // sprite mesh
    unsigned int numSpriteVertices;
    GLuint spriteVaoId;
    GLuint spriteVboId;

    // screen mesh
    unsigned int numScreenVertices;
    GLuint screenVaoId;
    GLuint screenVboId;

    // textures
    texture *waterDudvTexture;
    texture *waterNormalTexture;

    // renderables
    object **objects;
    unsigned long long numObjects;

    sun *sun;

    directional_light **directionalLights;
    unsigned long long numDirectionalLights;

    point_light **pointLights;
    unsigned long long numPointLights;

    spot_light **spotLights;
    unsigned long long numSpotLights;

    cubemap *skybox;

    water **waters;
    unsigned long long numWaters;

    sprite **sprites;
    unsigned long long numSprites;
};

#endif
