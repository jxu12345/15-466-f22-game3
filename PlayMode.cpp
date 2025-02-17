#include "PlayMode.hpp"

#include "LitColorTextureProgram.hpp"

#include "DrawLines.hpp"
#include "Mesh.hpp"
#include "Load.hpp"
#include "gl_errors.hpp"
#include "data_path.hpp"

#include <glm/gtc/type_ptr.hpp>

Load< Sound::Sample > dusty_floor_sample(LoadTagDefault, []() -> Sound::Sample const * {
	return new Sound::Sample(data_path("dusty-floor.opus"));
});

// load lock turning, opening and completion sound
Load< Sound::Sample > lock_turn_sample(LoadTagDefault, []() -> Sound::Sample const * {
	return new Sound::Sample(data_path("lock_turn.wav"));
});
Load< Sound::Sample > lock_open_sample(LoadTagDefault, []() -> Sound::Sample const * {
	return new Sound::Sample(data_path("lock_open.wav"));
});
Load< Sound::Sample > lock_solved_sample(LoadTagDefault, []() -> Sound::Sample const * {
	return new Sound::Sample(data_path("lock_solved.wav"));
});

GLuint lock_meshes_for_lit_color_texture_program = 0;
Load< MeshBuffer > lock_meshes(LoadTagDefault, []() -> MeshBuffer const * {
	MeshBuffer const *ret = new MeshBuffer(data_path("lock.pnct"));
	lock_meshes_for_lit_color_texture_program = ret->make_vao_for_program(lit_color_texture_program->program);
	return ret;
});

Load< Scene > lock_scene(LoadTagDefault, []() -> Scene const * {
	return new Scene(data_path("lock.scene"), [&](Scene &scene, Scene::Transform *transform, std::string const &mesh_name){
		Mesh const &mesh = lock_meshes->lookup(mesh_name);

		scene.drawables.emplace_back(transform);
		Scene::Drawable &drawable = scene.drawables.back();

		drawable.pipeline = lit_color_texture_program_pipeline;

		drawable.pipeline.vao = lock_meshes_for_lit_color_texture_program;
		drawable.pipeline.type = mesh.type;
		drawable.pipeline.start = mesh.start;
		drawable.pipeline.count = mesh.count;

	});
});


PlayMode::PlayMode() : scene(*lock_scene) {
	//get pointers to locks for convenience:
	for (auto &transform : scene.transforms) {
		if (transform.name == "Lock Module 1") lock[0] = &transform;
		if (transform.name == "Lock Module 2") lock[1] = &transform;
		if (transform.name == "Lock Module 3") lock[2] = &transform;
		if (transform.name == "Lock Module 4") lock[3] = &transform;
		if (transform.name == "Lock Module 5") lock[4] = &transform;
	}
	for (auto lock_transform : lock ) {
		if (lock_transform == nullptr) throw std::runtime_error("A Lock not found.");
	}

	//get pointer to camera for convenience:
	if (scene.cameras.size() != 1) throw std::runtime_error("Expecting scene to have exactly one camera, but it has " + std::to_string(scene.cameras.size()));
	camera = &scene.cameras.front();

	//start music loop playing:
	// (note: position will be over-ridden in update())
	// leg_tip_loop = Sound::loop_3D(*dusty_floor_sample, 1.0f, glm::vec3(0), 10.0f);

	// reset lock codes
	reset_locks();
}



PlayMode::~PlayMode() {
}

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {

	if (evt.type == SDL_KEYDOWN) {
		if (evt.key.keysym.sym == SDLK_ESCAPE) {
			SDL_SetRelativeMouseMode(SDL_FALSE);
			return true;
		} 
		else if (evt.key.keysym.sym == SDLK_a) {
			left.downs += 1;
			left.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_d) {
			right.downs += 1;
			right.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_w) {
			up.downs += 1;
			up.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_s) {
			down.downs += 1;
			down.pressed = true;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_LEFT) {
			arrowLeft.downs += 1;
			arrowLeft.pressed = true;
			return true;
		} 
		else if (evt.key.keysym.sym == SDLK_RIGHT) {
			arrowRight.downs += 1;
			arrowRight.pressed = true;
			return true;
		} 
		else if (evt.key.keysym.sym == SDLK_1) {
			k1.downs += 1;
			k1.pressed = true;
		}
		else if (evt.key.keysym.sym == SDLK_2) {
			k2.downs += 1;
			k2.pressed = true;
		}
		else if (evt.key.keysym.sym == SDLK_3) {
			k3.downs += 1;
			k3.pressed = true;
		}
		else if (evt.key.keysym.sym == SDLK_4) {
			k4.downs += 1;
			k4.pressed = true;
		}
		else if (evt.key.keysym.sym == SDLK_5) {
			k5.downs += 1;
			k5.pressed = true;
		}

	} else if (evt.type == SDL_KEYUP) {
		if (evt.key.keysym.sym == SDLK_a) {
			left.pressed = false;
			left.released = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_d) {
			right.pressed = false;
			right.released = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_w) {
			up.pressed = false;
			up.released = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_s) {
			down.pressed = false;
			down.released = true;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_LEFT) {
			arrowLeft.pressed = false;
			arrowLeft.released = true;
			return true;
		} 
		else if (evt.key.keysym.sym == SDLK_RIGHT) {
			arrowRight.pressed = false;
			arrowRight.released = true;
			return true;
		} 
		else if (evt.key.keysym.sym == SDLK_1) {
			k1.pressed = false;
			k1.released = true;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_2) {
			k2.pressed = false;
			k2.released = true;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_3) {
			k3.pressed = false;
			k3.released = true;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_4) {
			k4.pressed = false;
			k4.released = true;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_5) {
			k5.pressed = false;
			k5.released = true;
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_SPACE) {
			space.pressed = false;
			space.released = true;
			return true;
		}
	} else if (evt.type == SDL_MOUSEBUTTONDOWN) {
		if (SDL_GetRelativeMouseMode() == SDL_FALSE) {
			SDL_SetRelativeMouseMode(SDL_TRUE);
			return true;
		}
	} else if (evt.type == SDL_MOUSEMOTION) {
		if (SDL_GetRelativeMouseMode() == SDL_TRUE) {
			glm::vec2 motion = glm::vec2(
				evt.motion.xrel / float(window_size.y),
				-evt.motion.yrel / float(window_size.y)
			);
			camera->transform->rotation = glm::normalize(
				camera->transform->rotation
				* glm::angleAxis(-motion.x * camera->fovy, glm::vec3(0.0f, 1.0f, 0.0f))
				* glm::angleAxis(motion.y * camera->fovy, glm::vec3(1.0f, 0.0f, 0.0f))
			);
			return true;
		}
	}

	return false;
}

void PlayMode::update(float elapsed) {

	//move sound to follow leg tip position:
	// leg_tip_loop->set_position(get_leg_tip_position(), 1.0f / 60.0f);

	// update movement if no movement in progress
	static uint8_t lock_anim_iters = 0;
	static uint8_t lock_solved_iters = 0;

	static auto curr_rotation = glm::angleAxis(0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	constexpr float one_tenth_rotation = glm::radians(36.0f) / 10.0f;

	// play sound in middle of animation 
	if (lock_anim_iters == 10) {
		// print current lock codes and correct ones
		std::cout << "Current lock codes: " << std::to_string(current_code[0]) << std::to_string(current_code[1]) << std::to_string(current_code[2]) << std::to_string(current_code[3]) << std::to_string(current_code[4]) << std::endl;
		for (uint8_t i = 0; i < 5; i++) {
			if (i >= curr_lock) {
				// TODO check if lock code is correct
				bool correct = lock_correct(i);

				Sound::play_3D(
					*(correct ? lock_open_sample : lock_turn_sample), 
					1.0f - 0.5f * (!correct), 
					lock[i]-> make_local_to_world() * glm::vec4(lock[i]->position, 1.0f),
					2.0f
				);
			}
			
		}
	}

	if (lock_anim_iters > 0) {
		lock_anim_iters--;
		lock[curr_lock]->rotation = curr_rotation * lock[curr_lock]->rotation;
	}
	
	else {
		// reset locks on space key press
		if (space.released) {
			space.released = false;
			reset_locks();
		}
		// check if lock is already solved
		static bool lock_solved_sound_played = false;
		if (!lock_solved()) {
			lock_solved_sound_played = false;
			// switch locks
			if (k1.released) {
				k1.released = false;
				curr_lock = 0;
			}
			else if (k2.released) {
				k2.released = false;
				curr_lock = 1;
			}
			else if (k3.released) {
				k3.released = false;
				curr_lock = 2;
			}
			else if (k4.released) {
				k4.released = false;
				curr_lock = 3;
			}
			else if (k5.released) {
				k5.released = false;
				curr_lock = 4;
			}
			// record moves and start rotation animation
			if (arrowLeft.pressed) {
				curr_rotation = glm::angleAxis(-one_tenth_rotation / 2, glm::vec3(1.0f, 0.0f, 0.0f));
				left_turn(curr_lock);
				lock_anim_iters = 20;
			}
			if (arrowRight.pressed) {
				curr_rotation = glm::angleAxis(one_tenth_rotation / 2, glm::vec3(1.0f, 0.0f, 0.0f));
				right_turn(curr_lock);
				lock_anim_iters = 20;
			}
		}
		else if (!lock_solved_sound_played) {
			lock_solved_sound_played = true;
			Sound::play(
				*(lock_solved_sample),
				1.0f,
				0.0f
			);
		}
	}

	//move camera:
	{
		//combine inputs into a move:
		constexpr float PlayerSpeed = 15.0f;
		glm::vec2 move = glm::vec2(0.0f);
		if (left.pressed && !right.pressed) move.x =-1.0f;
		if (!left.pressed && right.pressed) move.x = 1.0f;
		if (down.pressed && !up.pressed) move.y =-1.0f;
		if (!down.pressed && up.pressed) move.y = 1.0f;

		//make it so that moving diagonally doesn't go faster:
		if (move != glm::vec2(0.0f)) move = glm::normalize(move) * PlayerSpeed * elapsed;

		glm::mat4x3 frame = camera->transform->make_local_to_parent();
		glm::vec3 frame_right = frame[0];
		//glm::vec3 up = frame[1];
		glm::vec3 frame_forward = -frame[2];

		camera->transform->position += move.x * frame_right + move.y * frame_forward;
	}

	{ //update listener to camera position:
		glm::mat4x3 frame = camera->transform->make_local_to_parent();
		glm::vec3 frame_right = frame[0];
		glm::vec3 frame_at = frame[3];
		Sound::listener.set_position_right(frame_at, frame_right, 1.0f / 60.0f);
	}

	//reset button press counters:
	left.downs = 0;
	right.downs = 0;
	up.downs = 0;
	down.downs = 0;
	arrowLeft.downs = 0;
	arrowRight.downs = 0;
	k1.downs = 0;
	k2.downs = 0;
	k3.downs = 0;
	k4.downs = 0;
	k5.downs = 0;
}

void PlayMode::draw(glm::uvec2 const &drawable_size) {
	//update camera aspect ratio for drawable:
	camera->aspect = float(drawable_size.x) / float(drawable_size.y);

	//set up light type and position for lit_color_texture_program:
	// TODO: consider using the Light(s) in the scene to do this
	glUseProgram(lit_color_texture_program->program);
	glUniform1i(lit_color_texture_program->LIGHT_TYPE_int, 1);
	glUniform3fv(lit_color_texture_program->LIGHT_DIRECTION_vec3, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f,-1.0f)));
	glUniform3fv(lit_color_texture_program->LIGHT_ENERGY_vec3, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 0.95f)));
	glUseProgram(0);

	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClearDepth(1.0f); //1.0 is actually the default value to clear the depth buffer to, but FYI you can change it.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS); //this is the default depth comparison function, but FYI you can change it.

	scene.draw(*camera);

	{ //use DrawLines to overlay some text:
		glDisable(GL_DEPTH_TEST);
		float aspect = float(drawable_size.x) / float(drawable_size.y) * 1.3f;
		DrawLines lines(glm::mat4(
			1.0f / aspect, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		));

		constexpr float H = 0.09f;

		std::string instructions = "Mouse rotates camera; WASD moves; esc ungrabs mouse; 1-5 changes locks; Arrow keys rotate locks; Space key resets combination";
		std::string completion = "Congratulations! You have solved the locks! Press space to reset the combination.";
		std::string str = lock_is_solved ? completion : instructions;
		lines.draw_text(str,
			glm::vec3(-aspect + 0.1f * H, -1.0 + 0.1f * H, 0.0),
			glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
			glm::u8vec4(0x00, 0x00, 0x00, 0x00));
		float ofs = 2.0f / drawable_size.y;
		lines.draw_text(str,
			glm::vec3(-aspect + 0.1f * H + ofs, -1.0 + + 0.1f * H + ofs, 0.0),
			glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
			glm::u8vec4(0xff, 0xff, 0xff, 0x00));
	}
	GL_ERRORS();
}
