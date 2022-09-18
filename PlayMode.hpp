#include "Mode.hpp"

#include "Scene.hpp"
#include "Sound.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>
#include <random> // for std::mt19937
#include <chrono> // for std::chrono


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

	// lock code and current code
	// accessed through getter and setter functions
	std::vector<uint8_t> lock_code = {0, 0, 0, 0, 0};
	std::vector<uint8_t> current_code = {0, 0, 0, 0, 0};
	uint8_t curr_lock = 0;

	// lock checking
	bool lock_correct (uint8_t lock_num) {
		return lock_code[lock_num] == current_code[lock_num];
	}

	bool lock_solved () {
		for (uint8_t i = 0; i < 5; i++) {
			if (!lock_correct(i)) {
				return false;
			}
		}
		return true;
	}

	// lock reset to random code and 0 user input
	void reset_locks() {
		// seed for random generator
		// random generator code from https://www.learncpp.com/cpp-tutorial/generating-random-numbers-using-mersenne-twister/
		std::mt19937 mt{ static_cast<unsigned int>(
			std::chrono::steady_clock::now().time_since_epoch().count()
		) };	
		for (uint8_t i = 0; i < 5; i++) {
			lock_code[i] = mt() % 10;
			assert(lock_code[i] < 10);
			current_code[i] = 0;
		}
	}

	// turn lock clockwise and counterclockwise
	void left_turn (uint8_t lock_num) { //counterclockwise, increment by 1
		if (lock_code[lock_num] == 9) {
			lock_code[lock_num] = 0;
		} else {
			lock_code[lock_num] += 1;
		}
	}

	void right_turn (uint8_t lock_num) { //clockwise, decrement by 1
		if (lock_code[lock_num] == 0) {
			lock_code[lock_num] = 9;
		} else {
			lock_code[lock_num] -= 1;
		}
	}

	
	//music coming from the tip of the leg (as a demonstration):
	std::shared_ptr< Sound::PlayingSample > leg_tip_loop;

	// lock turning and unlocking sounds
	
	//camera:
	Scene::Camera *camera = nullptr;

};
