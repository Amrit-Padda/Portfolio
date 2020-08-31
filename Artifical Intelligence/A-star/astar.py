import numpy as np
import time
from node import Node

#Helper function to find hueristic
def heuristic(child_x, child_y, end_x, end_y, straight, diagonal):
    dx = abs(child_x - end_x)
    dy = abs(child_y - end_y)
    return straight * (dx + dy) + (diagonal - 2 * straight) * min(dx, dy)

#Helper function to check the runtime of the algo
def checktime(start):
    if time.time() - start >= 10.0:
        return True
    else:
        return False  

#Helper function to return the path of the linked list
def return_path(current_node, hist):
    path = []
    num_rows, num_columns = np.shape(hist[0])
    result = hist[0]
    current = current_node
    while current is not None:
        path.append([current.x, current.y])
        current = current.parent
    
    path = path[::-1]
    return path

#Main function to find the path  
def search(hist, start, end, thres):
    startime = time.time()
    plot = hist[0]    
    start_node = Node(None, start[0], start[1])
    start_node.g = 0
    start_node.h = 0
    end_node = Node(None, end[0], end[1])
    end_node.g = 0
    end_node.h = 0

    yet_to_visit_list = []
    visited_list = []
    yet_to_visit_list.append(start_node)

    iterations = 0
    max_iterations = (len(plot) // 2) **3

    move = [[-1, 0],
            [0, -1],
            [1, 0],
            [0, 1],
            [-1, -1],
            [1, -1],
            [1, 1],
            [-1, 1]]

    no_rows, no_columns = np.shape(plot)
   
    while len(yet_to_visit_list) > 0:
        iterations += 1
        current_node = yet_to_visit_list[0]
        current_index = 0
        for index, item in enumerate(yet_to_visit_list):
            if item.f < current_node.f:
                current_node = item
                current_index = index

        if checktime(startime):
            print("Hit time limit")
            return return_path(current_node, hist)
        
        yet_to_visit_list.pop(current_index)
        visited_list.append(current_node)

        if current_node == end_node:
            return return_path(current_node, hist)
        
        children = []

        for new_position in move:
            #creating new node position
            node_x, node_y = (current_node.x + new_position[0],
                            current_node.y + new_position[1])
            
            #checking for illegal node
            if(node_x > (no_rows - 1) or node_x < 0 or
            node_y > (no_columns - 1) or node_y < 0):
                continue
            
            #checking for illegal moves
            if new_position == [1,1]:
                if plot[current_node.x][current_node.y] >= thres:
                    continue

            if new_position == [1,-1]:
                if plot[current_node.x][current_node.y - 1] >= thres:
                    continue

            if new_position == [-1,-1]:
                if plot[current_node.x - 1][current_node.y - 1] >= thres:
                    continue

            if new_position == [-1,1]:
                if plot[current_node.x - 1][current_node.y] >= thres:
                    continue
            
            if new_position == [1,0] :
                if plot[current_node.x][current_node.y] >= thres and plot[current_node.x][current_node.y - 1] >= thres:
                    continue
            
            if new_position == [-1,0]:
                if plot[current_node.x - 1][current_node.y] >= thres and plot[current_node.x - 1][current_node.y - 1] >= thres:
                    continue
            
            if new_position == [0,-1]:
                if plot[current_node.x][current_node.y - 1] >= thres and plot[current_node.x - 1][current_node.y - 1] >= thres:
                    continue

            if new_position == [0,1]:
                if plot[current_node.x][current_node.y] >= thres and plot[current_node.x - 1][current_node.y] >= thres:
                    continue
            
            
            #creating new node and checking for duplicate
            new_node = Node(current_node, node_x, node_y)
            if new_node in visited_list:
                continue
            
            #setting cost values
            cost = 0
            edge = 1.3
            diagonal = 1.5
            straight = 1


            #determining cost of movement
            if new_position == [-1,0]:
                if plot[current_node.x - 1][current_node.y] >= thres or plot[current_node.x - 1][current_node.y - 1] >= thres:
                    cost = edge
                else:
                    cost = straight

            if new_position == [0,-1]:
                if plot[current_node.x - 1][current_node.y - 1] >= thres or plot[current_node.x][current_node.y - 1] >= thres:
                    cost = edge
                else:
                    cost = straight
            
            if new_position == [1,0]:
                if plot[current_node.x][current_node.y] >= thres or plot[current_node.x][current_node.y - 1] >= thres:
                    cost = edge
                else:
                    cost = straight

            if new_position == [0,1]:
                if plot[current_node.x][current_node.y] >= thres or plot[current_node.x - 1][current_node.y] >= thres: 
                    cost = edge
                else:
                    cost = straight

            if new_position == [-1, -1] or new_position == [1, -1] or new_position == [1, 1] or new_position == [-1, 1]:
                cost = diagonal

            #calculating cost for movement
            new_node.g = current_node.g + cost
            new_node.h = heuristic(new_node.x, new_node.y, end_node.x, end_node.y, straight, diagonal)
            new_node.f = new_node.g + new_node.h

            #checking if better path exists
            if len([i for i in yet_to_visit_list if new_node == i and new_node.g > i.g]) > 0:
                continue

            yet_to_visit_list.append(new_node)

 



