// Local version of logic to convert sensor readings to desired motor output

#include <motor.hpp>

// class State {
//     public:
        
// }

mode current_mode = mode::start_exit;

enum class mode {
    start_exit,
    start_turn,
    follow_to_centre_cube,
    collect_cube, // includes collection of cube
    follow_to_tunnel,
    through_tunnel,
    follow_to_dropoff,
    dropoff_turn,
};

void start_exit() {
    // int line_hit_counter[] = {0,0,0,0};

    // if (line_hit_counter[0] == 2 && line_hit_counter[3] == 2

    // set_motor_speed(1, 200);
    // set_motor_speed(2, 200);
}

void start_turn() {}
void follow_to_centre_cube() {}
void collect_cube() {}
void follow_to_tunnel() {}
void through_tunnel() {}
void follow_to_dropoff() {}
void dropoff_turn() {}

void navigate() {
    switch (current_mode)
    {
    case mode::start_exit:
        start_exit();
        break;
    case mode::start_turn:
        start_turn();
        break;
    case mode::follow_to_centre_cube:
        follow_to_centre_cube();
        break;
    case mode::collect_cube:
        detect_cube();
        break;
    case mode::follow_to_tunnel:
        follow_to_tunnel();
        break;
    case mode::through_tunnel:
        through_tunnel();
        break;
    case mode::follow_to_dropoff:
        follow_to_dropoff();
        break;
    case mode::dropoff_turn:
        dropoff_turn();
        break;
    }
}