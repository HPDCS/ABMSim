#include <math.h>
#include <limits.h>
#include <strings.h>

#include "application.h"
#include "agent.h"
#include "region.h"



double a_star(agent_state_type *state, unsigned int current_cell, unsigned int *good_direction) {
	unsigned int i;
	double min_distance = INFTY;
	double current_distance;
	double dx, dy;
	unsigned int x1, y1, x2, y2;
	unsigned int tentative_cell;
	double distance_increment;

	*good_direction = UINT_MAX;
	SET_BIT(state->a_star_map, current_cell);
	map_linear_to_hexagon(state->target_cell, &x1, &y1);

	for(i = 0; i < CELL_EDGES; i ++) {

		// Query the simulation engine to retrieve the id of the region
		// heading in the selected direction
		tentative_cell = GetTargetRegion(current_cell, i);
		if (tentative_cell < 0) {
			// In that direction no region is reachable
			continue;
		}

		// We don't simply visit a cell already visited!
		if(CHECK_BIT(state->a_star_map, tentative_cell)) {
			continue;
		}

		// Can I go in that direction?
		// TODO: signed comparison
		if(state->visit_map[current_cell].neighbours[i] != -1) {

			// Is this the target?
			if(current_cell == state->target_cell) {
//				printf("TROVATO! current cell %d target %d\n", current_cell, state->target_cell);
				*good_direction = i;
				return 0.0;
			}

			// Compute the distance from the target if we make that move
			map_linear_to_hexagon(tentative_cell, &x2, &y2);
			dx = x2-x1;
			dy = y2-y1;
			
			distance_increment = a_star(state, tentative_cell, good_direction);
			// TODO: optimize?
			current_distance = sqrt( dx*dx + dy*dy );

			// Is it a good choice?
			if(current_distance < INFTY && current_distance < min_distance) {
				min_distance = current_distance;
				*good_direction = i;
			}
		}
	}

	return min_distance;
}


unsigned int compute_direction(agent_state_type *state) {
	unsigned int good_direction = UINT_MAX;

	bzero(state->a_star_map, BITMAP_SIZE(number_of_regions));

	a_star(state, state->current_cell, &good_direction);

	return good_direction;
}


unsigned int closest_frontier(agent_state_type *state, unsigned int exclude) {
	unsigned int i;
	unsigned int x, y, curr_x, curr_y;
	double distance;
	double min_distance = INFTY;
	unsigned int target = -1;
	unsigned int curr_cell = state->current_cell;

	map_linear_to_hexagon(curr_cell, &curr_x, &curr_y); 

	for(i = 0; i < number_of_regions; i++) {

		if(!state->visit_map[i].visited) {

			if(i == exclude) {
				// I know this is a frontier, but I don't want to go there
				continue;
			}
			
			map_linear_to_hexagon(i, &x, &y);
			// TODO: optimize
			distance = sqrt((curr_x - x)*(curr_x - x) + (curr_y - y)*(curr_y - y));

			if(distance < min_distance) {
				min_distance = distance;
				target = i;
			}
		}
	}

	return target;
}