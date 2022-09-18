#include "Mode.hpp"

#include "Scene.hpp"
#include "Sound.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>

struct PlayMode : Mode {
	PlayMode();
	virtual ~PlayMode();

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	//----- game state -----

	//input tracking:
	struct Button {
		uint8_t downs = 0;
		uint8_t pressed = 0;
		uint8_t released = 0;
	} up, down, left, right, arrowLeft, arrowRight, k1, k2, k3, k4, k5;

	// vector storing number keys
	std::vector<Button> num_keys = std::vector<Button>(5);

	//local copy of the game scene (so code can change it during gameplay):
	Scene scene;

	// locks and their rotations
	std::vector<Scene::Transform *> lock = {nullptr, nullptr, nullptr, nullptr, nullptr};
	std::vector<glm::quat> lock_rotation = {glm::quat(), glm::quat(), glm::quat(), glm::quat(), glm::quat()};

	// lock code and current code
	// accessed through getter and setter functions
	std::vector<uint8_t> lock_code = {0, 0, 0, 0, 0};
	std::vector<uint8_t> current_code = {0, 0, 0, 0, 0};
	uint8_t curr_lock = 0;

	void check_locks();

	void clkwise_turn (uint8_t lock_num) {
		if (lock_code[lock_num] == 9) {
			lock_code[lock_num] = 0;
		} else {
			lock_code[lock_num] += 1;
		}
	}

	void counter_clkwise_turn (uint8_t lock_num) {
		if (lock_code[lock_num] == 0) {
			lock_code[lock_num] = 9;
		} else {
			lock_code[lock_num] -= 1;
		}
	}

	
	//music coming from the tip of the leg (as a demonstration):
	std::shared_ptr< Sound::PlayingSample > leg_tip_loop;
	
	//camera:
	Scene::Camera *camera = nullptr;

};
