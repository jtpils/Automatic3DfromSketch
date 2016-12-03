//
//  polygon.cpp
//  sketch
//
//  Created by Pranjal Sahu on 10/29/16.
//  Copyright © 2016 Pranjal Sahu. All rights reserved.
//

#include <stdio.h>
#include "myline.h"
#include "polygon.h"


polygon::polygon(std::vector<myline*> tlines){
    this->lines = tlines;
    placed = false;
    axis_assigned = false;
}


// checks if two polygons are equal or not
bool polygon::is_equal_to(polygon *b){
    std::vector<myline*> at = this->lines;
    std::vector<myline*> bt = b->lines;
    
    if(bt.size() != at.size()){
        return false;
    }
    
    bool flag = false;
    for(std::vector<myline*>::iterator it = bt.begin(); it != bt.end(); ++it){
        myline *t = *it;
        flag = false;
        for(std::vector<myline*>::iterator ita = at.begin(); ita != at.end(); ++ita){
            myline *ta = *ita;
            if(ta->is_equal_to(t) || ta->is_reverse_of(t)){
                flag = true;
                break;
            }
        }
        
        if(flag == false){
            return false;
        }
    }
    
    return true;
}

// checks if the polygon is contained in the polygon vector pvector
bool polygon::is_part_of(std::vector<polygon*> pvector){
    for(std::vector<polygon*>::iterator it = pvector.begin(); it != pvector.end(); ++it){
        polygon *a = *it;
        if(this->is_equal_to(a)){
            return true;
        }
    }
    return false;
}

// returns all the polygons by traversing taking each line segment as starting point
std::vector<polygon*> get_all_polygons(std::vector<myline*> all_lines){
    std::vector<polygon*> all_polygons;
    // loop for every line and take it as start and get the polygon by traversing over all valid lines
    for(std::vector<myline*>::iterator it = all_lines.begin(); it != all_lines.end(); ++it){
        myline *start = *it;
        std::vector<myline*> polygon_lines = start->get_polygon(all_lines);
        polygon* temp = new polygon(polygon_lines);
        
        // check if this polygon is already covered
        // if not then insert it into list of polygons
        if(!temp->is_part_of(all_polygons)){
            all_polygons.push_back(temp);
        }
    }
    
    return all_polygons;
}


// returns the points of a polygon by traversing over the lines
std::vector<mypoint*> polygon::get_points(){
    std::vector<mypoint*> all_points;
    for(std::vector<myline*>::iterator it = lines.begin(); it != lines.end(); ++it){
        myline *l = *it;
        all_points.push_back(new mypoint(l->x1, l->y1, 0));
    }
    return all_points;
}

// returns the points of a polygon by traversing over the lines
std::vector<glm::vec2> polygon::get_points_vec(){
    std::vector<glm::vec2> all_points;
    for(std::vector<myline*>::iterator it = lines.begin(); it != lines.end(); ++it){
        myline *l = *it;
        all_points.push_back(glm::vec2(l->x1, l->y1));
    }
    return all_points;
}

std::vector<myline*> polygon::get_green_lines(){
    std::vector<myline*> green_lines;
    for(std::vector<myline*>::iterator it = lines.begin(); it != lines.end(); ++it){
        myline *t = *it;
        if(t->label.compare("green") == 0){
            green_lines.push_back(t);
        }
    }
    return green_lines;
}

// returns the three points of the starting plane
std::vector<glm::vec3> polygon::get_plane_points(polygon *p, myline * line, std::vector<glm::vec2> corres_2d, std::vector<glm::vec3> corres_3d){
    std::vector<glm::vec3> po;
    std::vector<myline*> lines = this->lines;
    myline *line2;  // get the next line
    
    int i=0;
    for(i=0;i<lines.size();++i){
        if(lines[i]->is_equal_to(line) || lines[i]->is_reverse_of(line)){
            break;
        }
    }
    
    
    if(i ==lines.size()-1){
        line2 = lines[0];
    }
    else{
        line2 = lines[i+1];
    }
    
    // first two points will be rotation axis
    for(int i=0;i<corres_2d.size();++i){
        if(line->x1 == corres_2d[i][0] && line->y1 == corres_2d[i][1]){
            po.push_back(corres_3d[i]);
        }
        if(line->x2 == corres_2d[i][0] && line->y2 == corres_2d[i][1]){
            po.push_back(corres_3d[i]);
        }
    }
    for(int i=0;i<corres_2d.size();++i){
        if(line2->x2 == corres_2d[i][0] && line2->y2 == corres_2d[i][1]){
            po.push_back(corres_3d[i]);
        }
    }
    
    return po;
}


// returns the adjacent polygon of this polygon using the huffman labels
// i.e. return polygons attached by a green line
std::vector<polygon*> polygon::get_adjacent_polygons_using_huffman(std::vector<polygon*> all_polygons,
                                                                   std::vector<glm::vec2> corres_2d,
                                                                   std::vector<glm::vec3> corres_3d){
    std::vector<polygon*> adjacent_polygons;
    
    // get the convex lines
    std::vector<myline*> green_lines = this->get_green_lines();
    
    // get all polygons which share edge using green lines
    for(std::vector<polygon*>::iterator it = all_polygons.begin(); it != all_polygons.end(); ++it){
        polygon * pp = *it;
        std::vector<myline*> plines  = pp->lines;
        
        for(std::vector<myline*>::iterator itb = plines.begin(); itb != plines.end(); ++itb){
            myline * lp = *itb;
            
            bool flag = false;
            //if it shares any green line with the current polygon lines then push it
            for(std::vector<myline*>::iterator ita = green_lines.begin(); ita != green_lines.end(); ++ita){
                myline *lpp = *ita;
                if((lp == lpp || lp->reverse_line == lpp) && !this->is_equal_to(pp)){
                    
                    if(!pp->axis_assigned){
                        // calculate the rotation axis for this adjacent polygon
                        std::vector<glm::vec3> plane_points = get_plane_points(pp, lpp, corres_2d, corres_3d);
                        
                        // get normalized rotation axis
                        glm::vec3 rotation_axis = plane_points[0]-plane_points[1];
                        rotation_axis = rotation_axis/glm::length(rotation_axis);
                        
                        
                        plane * np = new plane(plane_points);
                        
                        pp->rotation_axis    = rotation_axis;
                        pp->plane_to_project = np;
                        pp->axis_assigned    = true;
                    }
                    
                    adjacent_polygons.push_back(pp);
                    flag = true;
                    break;
                }
            }
            
            if(flag){
                break;
            }
        }
    }
    
    
    return adjacent_polygons;
}