#ifndef GAME_RENDERER_HPP
#define GAME_RENDERER_HPP

struct Camera;
struct Cubemap;
struct DirectionalLight;
struct Object;
struct PointLight;
class Program;
struct SpotLight;
struct Sprite;
struct Sun;
struct Texture;
struct Water;

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

    void setMode(RenderMode renderMode);
    void addObject(Object *object);
    void setSun(Sun *sun);
    void addDirectionalLight(DirectionalLight *directionalLight);
    void addPointLight(PointLight *pointLight);
    void addSpotLight(SpotLight *spotLight);
    void setSkybox(Cubemap *skybox);
    void addWater(Water *water);
    void addSprite(Sprite *sprite);
    void draw(Camera *camera, float aspect, unsigned int elapsedTime, float deltaTime) const;

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
    RenderMode renderMode;

    // shader programs
    Program *depthProgram;
    Program *forwardColorProgram;
    Program *forwardSunProgram;
    Program *forwardDirectionalProgram;
    Program *forwardPointProgram;
    Program *forwardSpotProgram;
    Program *forwardReflectionProgram;
    Program *geometryProgram;
    Program *deferredSunProgram;
    Program *deferredDirectionalProgram;
    Program *deferredPointProgram;
    Program *deferredSpotProgram;
    Program *skyboxProgram;
    Program *waterProgram;
    Program *spriteProgram;
    Program *screenProgram;

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
    Texture *waterDudvTexture;
    Texture *waterNormalTexture;

    // renderables
    std::vector<Object *> objects;
    Sun *sun;
    std::vector<DirectionalLight *> directionalLights;
    std::vector<PointLight *> pointLights;
    std::vector<SpotLight *> spotLights;
    Cubemap *skybox;
    std::vector<Water *> waters;
    std::vector<Sprite *> sprites;
};

#endif
