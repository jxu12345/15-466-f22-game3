#include "Mode.hpp"

#include "Scene.hpp"
#include "Sound.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>
#include <random> // for std::mt19937
#include <chrono> // for std::chrono
#include <iostream>

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
	} up, down, left, right, arrowLeft, arrowRight, k1, k2, k3, k4, k5, space;

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
	bool lock_is_solved = false;

	// lock checking
	bool lock_correct (uint8_t lock_num) {
		return lock_code[lock_num] == current_code[lock_num];
	}

	bool lock_solved () {
		for (uint8_t i = 0; i < 5; i++) {
			if (!lock_correct(i)) {
				lock_is_solved = false;
				return false;
			}
		}
		lock_is_solved = true;
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
		std::cout << "Correct lock codes: " << std::to_string(lock_code[0]) << std::to_string(lock_code[1]) << std::to_string(lock_code[2]) << std::to_string(lock_code[3]) << std::to_string(lock_code[4]) << std::endl;
	}

	// turn lock clockwise and counterclockwise
	void right_turn (uint8_t lock_num) { //clockwise, increment by 1
		// turn this lock and all next locks
		for (int i = lock_num; i < 5; i++) {
			if (current_code[i] == 9) {
				current_code[i] = 0;
			} else {
				current_code[i] += 1;
			}
		}
	}

	void left_turn (uint8_t lock_num) { //counterclockwise, decrement by 1
		// turn this lock and all next locks
		for (int i = lock_num; i < 5; i++) {
			if (current_code[i] == 0) {
				current_code[i] = 9;
			} else {
				current_code[i] -= 1;
			}
		}
	}

	
	//music coming from the tip of the leg (as a demonstration):
	std::shared_ptr< Sound::PlayingSample > leg_tip_loop;
	
	//camera:
	Scene::Camera *camera = nullptr;

};
