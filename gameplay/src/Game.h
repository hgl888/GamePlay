#ifndef GAME_H_
#define GAME_H_

#ifdef _WIN32
#include <windows.h>
#include <fcntl.h>
#include <io.h>
#elif defined(__ANDROID__)
#include <android/native_activity.h>
#include <android/asset_manager.h>
#include <android_native_app_glue.h>
#include "vulkanandroid.h"
#elif defined(__linux__)
#include <xcb/xcb.h>
#endif

#include <iostream>
#include <chrono>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <string>
#include <array>

#include "vulkan/vulkan.h"

//#include "keycodes.hpp"
#include "vulkantools.h"
#include "vulkandebug.h"

#include "vulkandevice.hpp"
#include "vulkanswapchain.hpp"
#include "vulkanTextureLoader.hpp"
#include "vulkanMeshLoader.hpp"
#include "vulkantextoverlay.hpp"
#include "vkcamera.hpp"
///////////////////////

#include "Keyboard.h"
#include "Mouse.h"
#include "Touch.h"
#include "Gesture.h"
#include "Gamepad.h"
#include "AudioController.h"
#include "AnimationController.h"
#include "PhysicsController.h"
#include "AIController.h"
#include "AudioListener.h"
#include "VkRectangle.h"
#include "Vector4.h"
#include "TimeListener.h"

typedef VkPhysicalDeviceFeatures(*PFN_GetEnabledFeatures)();

namespace vk
{

class ScriptController;

class Game
{

private:
	bool mEnableValidation = false;
	bool mEnableVSync = false;
	VkPhysicalDeviceFeatures mEnabledFeatures = {};
	float mFpsTimer = 0.0f;
	bool mViewUpdated = false;
	uint32_t destWidth;
	uint32_t destHeight;
	bool resizing = false;
	// Called if the window is resized and some resources have to be recreatesd

	VkResult createInstance(bool enableValidation);
	std::string getWindowTitle();
	void windowResize();
protected:
	// Last frame time, measured using a high performance timer (if available)
	float frameTimer = 1.0f;
	// Frame counter to display fps
	uint32_t frameCounter = 0;
	uint32_t lastFPS = 0;
	// Vulkan instance, stores all per-application states
	VkInstance mInstance;
	// Physical device (GPU) that Vulkan will ise
	VkPhysicalDevice mPhysicalDevice;
	// Stores physical device properties (for e.g. checking device limits)
	VkPhysicalDeviceProperties mDeviceProperties;
	// Stores phyiscal device features (for e.g. checking if a feature is available)
	VkPhysicalDeviceFeatures mDeviceFeatures;
	// Stores all available memory (type) properties for the physical device
	VkPhysicalDeviceMemoryProperties mDeviceMemoryProperties;
	/** @brief Logical device, application's view of the physical device (GPU) */
	// todo: getter? should always point to VulkanDevice->device
	VkDevice mDevice;
	/** @brief Encapsulated physical and logical vulkan device */
	vk::VulkanDevice *mVulkanDevice;
	// Handle to the device graphics queue that command buffers are submitted to
	VkQueue mQueue;
	// Color buffer format
	VkFormat colorformat = VK_FORMAT_B8G8R8A8_UNORM;
	// Depth buffer format
	// Depth format is selected during Vulkan initialization
	VkFormat depthFormat;
	// Command buffer pool
	VkCommandPool cmdPool;
	// Command buffer used for setup
	VkCommandBuffer setupCmdBuffer = VK_NULL_HANDLE;
	/** @brief Pipeline stages used to wait at for graphics queue submissions */
	VkPipelineStageFlags submitPipelineStages = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	// Contains command buffers and semaphores to be presented to the queue
	VkSubmitInfo mSubmitInfo;
	// Command buffers used for rendering
	std::vector<VkCommandBuffer> mDrawCmdBuffers;
	// Global render pass for frame buffer writes
	VkRenderPass mRenderPass;
	// List of available frame buffers (same as number of swap chain images)
	std::vector<VkFramebuffer>frameBuffers;
	// Active frame buffer index
	uint32_t mCurrentBuffer = 0;
	// Descriptor set pool
	VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
	// List of shader modules created (stored for cleanup)
	std::vector<VkShaderModule> shaderModules;
	// Pipeline cache object
	VkPipelineCache pipelineCache;
	// Wraps the swap chain to present images (framebuffers) to the windowing system
	VulkanSwapChain mSwapChain;
	// Synchronization semaphores
	struct {
		// Swap chain image presentation
		VkSemaphore presentComplete;
		// Command buffer submission and execution
		VkSemaphore renderComplete;
		// Text overlay submission and execution
		VkSemaphore textOverlayComplete;
	} mSemaphores;
	// Simple texture loader
	vkTools::VulkanTextureLoader *textureLoader = nullptr;
	// Returns the base asset path (for shaders, models, textures) depending on the os
	const std::string getAssetPath();
public:
	bool prepared = false;
	uint32_t width = 1280;
	uint32_t height = 720;

	VkClearColorValue defaultClearColor = { { 0.025f, 0.025f, 0.025f, 1.0f } };

	float zoom = 0;

	// Defines a frame rate independent timer value clamped from -1.0...1.0
	// For use in animations, rotations, etc.
	float timer = 0.0f;
	// Multiplier for speeding up (or slowing down) the global timer
	float timerSpeed = 0.25f;

	bool paused = false;

	bool enableTextOverlay = false;
	VulkanTextOverlay *mTextOverlay;

	// Use to adjust mouse rotation speed
	float rotationSpeed = 1.0f;
	// Use to adjust mouse zoom speed
	float zoomSpeed = 1.0f;

	VkCamera camera;

	glm::vec3 rotation = glm::vec3();
	glm::vec3 cameraPos = glm::vec3();
	glm::vec2 mousePos;

	std::string title = "Vulkan Example";
	std::string name = "vulkanExample";

	struct
	{
		VkImage image;
		VkDeviceMemory mem;
		VkImageView view;
	} depthStencil;

	// Gamepad state (only one pad supported)
	struct
	{
		glm::vec2 axisLeft = glm::vec2(0.0f);
		glm::vec2 axisRight = glm::vec2(0.0f);
	} gamePadState;

	// OS specific 
#if defined(_WIN32)
	HWND window;
	HINSTANCE windowInstance;
#elif defined(__ANDROID__)
	android_app* androidApp;
	// true if application has focused, false if moved to background
	bool focused = false;
#elif defined(__linux__)
	struct {
		bool left = false;
		bool right = false;
		bool middle = false;
	} mouseButtons;
	bool quit = false;
	xcb_connection_t *connection;
	xcb_screen_t *screen;
	xcb_window_t window;
	xcb_intern_atom_reply_t *atom_wm_delete_window;
#endif

	virtual void InitVulkanExample(bool enableValidation, PFN_GetEnabledFeatures enabledFeaturesFn = nullptr);

	virtual void UnInitVulkanExample();

	// Setup the vulkan instance, enable required extensions and connect to the physical device (GPU)
	void initVulkan(bool enableValidation);

#if defined(_WIN32)
	void setupConsole(std::string title);
	HWND setupWindow(HINSTANCE hinstance, WNDPROC wndproc);
	void handleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
#elif defined(__ANDROID__)
	static int32_t handleAppInput(struct android_app* app, AInputEvent* event);
	static void handleAppCommand(android_app* app, int32_t cmd);
#elif defined(__linux__)
	xcb_window_t setupWindow();
	void initxcbConnection();
	void handleEvent(const xcb_generic_event_t *event);
#endif
	// Pure virtual render function (override in derived class)
	virtual void render();
	// Called when view change occurs
	// Can be overriden in derived class to e.g. update uniform buffers 
	// Containing view dependant matrices
	virtual void viewChanged();
	// Called if a key is pressed
	// Can be overriden in derived class to do custom key handling
	virtual void keyPressed(uint32_t keyCode);
	// Called when the window has been resized
	// Can be overriden in derived class to recreate or rebuild resources attached to the frame buffer / swapchain
	virtual void windowResized();
	// Pure virtual function to be overriden by the dervice class
	// Called in case of an event where e.g. the framebuffer has to be rebuild and thus
	// all command buffers that may reference this
	virtual void buildCommandBuffers();

	// Creates a new (graphics) command pool object storing command buffers
	void createCommandPool();
	// Setup default depth and stencil views
	virtual void setupDepthStencil();
	// Create framebuffers for all requested swap chain images
	// Can be overriden in derived class to setup a custom framebuffer (e.g. for MSAA)
	virtual void setupFrameBuffer();
	// Setup a default render pass
	// Can be overriden in derived class to setup a custom render pass (e.g. for MSAA)
	virtual void setupRenderPass();

	// Connect and prepare the swap chain
	void initSwapchain();
	// Create swap chain images
	void setupSwapChain();

	// Check if command buffers are valid (!= VK_NULL_HANDLE)
	bool checkCommandBuffers();
	// Create command buffers for drawing commands
	void createCommandBuffers();
	// Destroy all command buffers and set their handles to VK_NULL_HANDLE
	// May be necessary during runtime if options are toggled 
	void destroyCommandBuffers();
	// Create command buffer for setup commands
	void createSetupCommandBuffer();
	// Finalize setup command bufferm submit it to the queue and remove it
	void flushSetupCommandBuffer();

	// Command buffer creation
	// Creates and returns a new command buffer
	VkCommandBuffer createCommandBuffer(VkCommandBufferLevel level, bool begin);
	// End the command buffer, submit it to the queue and free (if requested)
	// Note : Waits for the queue to become idle
	void flushCommandBuffer(VkCommandBuffer commandBuffer, VkQueue queue, bool free);

	// Create a cache pool for rendering pipelines
	void createPipelineCache();

	// Prepare commonly used Vulkan functions
	virtual void prepare();

	// Load a SPIR-V shader
	VkPipelineShaderStageCreateInfo loadShader(std::string fileName, VkShaderStageFlagBits stage);

	// Create a buffer, fill it with data (if != NULL) and bind buffer memory
	VkBool32 createBuffer(
		VkBufferUsageFlags usageFlags,
		VkMemoryPropertyFlags memoryPropertyFlags,
		VkDeviceSize size,
		void *data,
		VkBuffer *buffer,
		VkDeviceMemory *memory);
	// This version always uses HOST_VISIBLE memory
	VkBool32 createBuffer(
		VkBufferUsageFlags usage,
		VkDeviceSize size,
		void *data,
		VkBuffer *buffer,
		VkDeviceMemory *memory);
	// Overload that assigns buffer info to descriptor
	VkBool32 createBuffer(
		VkBufferUsageFlags usage,
		VkDeviceSize size,
		void *data,
		VkBuffer *buffer,
		VkDeviceMemory *memory,
		VkDescriptorBufferInfo *descriptor);
	// Overload to pass memory property flags
	VkBool32 createBuffer(
		VkBufferUsageFlags usage,
		VkMemoryPropertyFlags memoryPropertyFlags,
		VkDeviceSize size,
		void *data,
		VkBuffer *buffer,
		VkDeviceMemory *memory,
		VkDescriptorBufferInfo *descriptor);

	// Load a mesh (using ASSIMP) and create vulkan vertex and index buffers with given vertex layout
	void loadMesh(
		std::string fiename,
		vkMeshLoader::MeshBuffer *meshBuffer,
		std::vector<vkMeshLoader::VertexLayout> vertexLayout,
		float scale);
	void loadMesh(
		std::string filename,
		vkMeshLoader::MeshBuffer *meshBuffer,
		std::vector<vkMeshLoader::VertexLayout>
		vertexLayout,
		vkMeshLoader::MeshCreateInfo *meshCreateInfo);

	// Start the main render loop
	void renderLoop();

	void updateTextOverlay();

	// Called when the text overlay is updating
	// Can be overriden in derived class to add custom text to the overlay
	virtual void getOverlayText(VulkanTextOverlay * textOverlay);

	// Prepare the frame for workload submission
	// - Acquires the next image from the swap chain 
	// - Sets the default wait and signal semaphores
	void prepareFrame();

	// Submit the frames' workload 
	// - Submits the text overlay (if enabled)
	void submitFrame();

//////////////////////////////////////////////////////////////////////////
private:
    friend class Platform;
    friend class Gamepad;
    friend class ShutdownListener;

public:
    
    enum State
    {
        UNINITIALIZED,
        RUNNING,
        PAUSED
    };

    enum ClearFlags
    {
        CLEAR_COLOR = GL_COLOR_BUFFER_BIT,
        CLEAR_DEPTH = GL_DEPTH_BUFFER_BIT,
        CLEAR_STENCIL = GL_STENCIL_BUFFER_BIT,
        CLEAR_COLOR_DEPTH = CLEAR_COLOR | CLEAR_DEPTH,
        CLEAR_COLOR_STENCIL = CLEAR_COLOR | CLEAR_STENCIL,
        CLEAR_DEPTH_STENCIL = CLEAR_DEPTH | CLEAR_STENCIL,
        CLEAR_COLOR_DEPTH_STENCIL = CLEAR_COLOR | CLEAR_DEPTH | CLEAR_STENCIL
    };
    
    Game();
    virtual ~Game();
    static Game* getInstance();
    static bool isVsync();

    static void setVsync(bool enable);
    static double getAbsoluteTime();

    static double getGameTime();
    inline State getState() const;
    inline bool isInitialized() const;

    Properties* getConfig() const;

    int run();
    void pause();
	void resume();
    void exit();
    void frame();
    inline unsigned int getFrameRate() const;
    inline unsigned int getWidth() const;
    inline unsigned int getHeight() const;
    inline float getAspectRatio() const;
    inline const VkRectangle& getViewport() const;
    void setViewport(const VkRectangle& viewport);
  
    void clear(ClearFlags flags, const Vector4& clearColor, float clearDepth, int clearStencil);
   
    void clear(ClearFlags flags, float red, float green, float blue, float alpha, float clearDepth, int clearStencil);

    inline AudioController* getAudioController() const;

    inline AnimationController* getAnimationController() const;

    inline PhysicsController* getPhysicsController() const;

    inline AIController* getAIController() const;

    inline ScriptController* getScriptController() const;

    AudioListener* getAudioListener();
    
     inline void displayKeyboard(bool display);
     
    virtual void keyEvent(Keyboard::KeyEvent evt, int key);

    virtual void touchEvent(Touch::TouchEvent evt, int x, int y, unsigned int contactIndex);

    virtual bool mouseEvent(Mouse::MouseEvent evt, int x, int y, int wheelDelta);
    
    virtual void resizeEvent(unsigned int width, unsigned int height);

    inline bool hasMouse();
    
    inline bool isMouseCaptured();
    
    inline void setMouseCaptured(bool captured);
    
    inline void setCursorVisible(bool visible);
    
    inline bool isCursorVisible();

    bool isGestureSupported(Gesture::GestureEvent evt);

    void registerGesture(Gesture::GestureEvent evt);

    void unregisterGesture(Gesture::GestureEvent evt);

    bool isGestureRegistered(Gesture::GestureEvent evt);

    virtual void gestureSwipeEvent(int x, int y, int direction);

    virtual void gesturePinchEvent(int x, int y, float scale);

    virtual void gestureLongTapEvent(int x, int y, float duration);

    virtual void gestureTapEvent(int x, int y);

    virtual void gestureDragEvent(int x, int y);

    virtual void gestureDropEvent(int x, int y);

    virtual void gamepadEvent(Gamepad::GamepadEvent evt, Gamepad* gamepad);

    inline unsigned int getGamepadCount() const;

    inline Gamepad* getGamepad(unsigned int index, bool preferPhysical = true) const;

    inline void setMultiSampling(bool enabled);

    inline bool isMultiSampling() const;

    inline void setMultiTouch(bool enabled);

    inline bool isMultiTouch() const;

    inline bool canExit() const;

    inline bool hasAccelerometer() const;

    inline void getAccelerometerValues(float* pitch, float* roll);

    inline void getSensorValues(float* accelX, float* accelY, float* accelZ, float* gyroX, float* gyroY, float* gyroZ);

    void getArguments(int* argc, char*** argv) const;

    void schedule(float timeOffset, TimeListener* timeListener, void* cookie = 0);

    void schedule(float timeOffset, const char* function);

    void clearSchedule();
  
    bool launchURL(const char *url) const;

protected:

    virtual void initialize();
   
    virtual void finalize();
 
    virtual void update(float elapsedTime);

    virtual void render(float elapsedTime);
 
    template <class T>
    void renderOnce(T* instance, void (T::*method)(void*), void* cookie);

    void renderOnce(const char* function);
   
    void updateOnce();

private:

    struct ShutdownListener : public TimeListener
    {
        void timeEvent(long timeDiff, void* cookie);
    };

    /**
     * TimeEvent represents the event that is sent to TimeListeners as a result of calling Game::schedule().
     */
    class TimeEvent
    {
    public:

        TimeEvent(double time, TimeListener* timeListener, void* cookie);
        bool operator<(const TimeEvent& v) const;
        double time;
        TimeListener* listener;
        void* cookie;
    };

   
    Game(const Game& copy);

    bool startup();

    void shutdown();
    void fireTimeEvents(double frameTime);
    void loadConfig();
   
    void loadGamepads();

    void keyEventInternal(Keyboard::KeyEvent evt, int key);
    void touchEventInternal(Touch::TouchEvent evt, int x, int y, unsigned int contactIndex);
    bool mouseEventInternal(Mouse::MouseEvent evt, int x, int y, int wheelDelta);
    void resizeEventInternal(unsigned int width, unsigned int height);
    void gestureSwipeEventInternal(int x, int y, int direction);
    void gesturePinchEventInternal(int x, int y, float scale);
    void gestureTapEventInternal(int x, int y);
    void gestureLongTapEventInternal(int x, int y, float duration);
    void gestureDragEventInternal(int x, int y);
    void gestureDropEventInternal(int x, int y);
    void gamepadEventInternal(Gamepad::GamepadEvent evt, Gamepad* gamepad);

    bool _initialized;                          // If game has initialized yet.
    State _state;                               // The game state.
    unsigned int _pausedCount;                  // Number of times pause() has been called.
    static double _pausedTimeLast;              // The last time paused.
    static double _pausedTimeTotal;             // The total time paused.
    double _frameLastFPS;                       // The last time the frame count was updated.
    unsigned int _frameCount;                   // The current frame count.
    unsigned int _frameRate;                    // The current frame rate.
    unsigned int _width;                        // The game's display width.
    unsigned int _height;                       // The game's display height.
    VkRectangle _viewport;                        // the games's current viewport.
    Vector4 _clearColor;                        // The clear color value last used for clearing the color buffer.
    float _clearDepth;                          // The clear depth value last used for clearing the depth buffer.
    int _clearStencil;                          // The clear stencil value last used for clearing the stencil buffer.
    Properties* _properties;                    // Game configuration properties object.
    AnimationController* _animationController;  // Controls the scheduling and running of animations.
    AudioController* _audioController;          // Controls audio sources that are playing in the game.
    PhysicsController* _physicsController;      // Controls the simulation of a physics scene and entities.
    AIController* _aiController;                // Controls AI simulation.
    AudioListener* _audioListener;              // The audio listener in 3D space.
    std::priority_queue<TimeEvent, std::vector<TimeEvent>, std::less<TimeEvent> >* _timeEvents;     // Contains the scheduled time events.
    ScriptController* _scriptController;            // Controls the scripting engine.
    ScriptTarget* _scriptTarget;                // Script target for the game

    // Note: Do not add STL object member variables on the stack; this will cause false memory leaks to be reported.

    friend class ScreenDisplayer;
};

}

#include "Game.inl"

#endif
