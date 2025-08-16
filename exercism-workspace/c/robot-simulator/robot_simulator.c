#include "robot_simulator.h"
#include <string.h>

robot_status_t robot_create(robot_direction_t direction, int x, int y)
{
    robot_status_t robot;
    robot.direction = direction;
    robot.position.x = x;
    robot.position.y = y;
    return robot;
}

void robot_move(robot_status_t *robot, const char *commands)
{
    if (robot == NULL || commands == NULL) {
        return;
    }
    
    for (size_t i = 0; i < strlen(commands); i++) {
        char command = commands[i];
        
        switch (command) {
            case 'R':
                // Turn right (clockwise)
                robot->direction = (robot->direction + 1) % DIRECTION_MAX;
                break;
            case 'L':
                // Turn left (counter-clockwise)
                robot->direction = (robot->direction + DIRECTION_MAX - 1) % DIRECTION_MAX;
                break;
            case 'A':
                // Advance based on current direction
                switch (robot->direction) {
                    case DIRECTION_NORTH:
                        robot->position.y++;
                        break;
                    case DIRECTION_EAST:
                        robot->position.x++;
                        break;
                    case DIRECTION_SOUTH:
                        robot->position.y--;
                        break;
                    case DIRECTION_WEST:
                        robot->position.x--;
                        break;
                    default:
                        break;
                }
                break;
            default:
                // Ignore unknown commands
                break;
        }
    }
}
