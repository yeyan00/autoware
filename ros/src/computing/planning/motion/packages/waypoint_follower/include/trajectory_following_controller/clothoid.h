#ifndef WAYPOINT_FOLLWER_CLOTHOID_H_INCLUDED
#define WAYPOINT_FOLLWER_CLOTHOID_H_INCLUDED

/*
 * Copyright 2015-2019 Autoware Foundation. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

class Clothoid
{
public:
    Clothoid();
    ~Clothoid();
    /*
    h : length of the Clothoid
    phi0 : start angle
    phiV : the incremental of the angle if the curventure was not chenged
    phiU : the incremental of the angle
    */
    void draw(double h,double phi0,double phiV,double phiU);
};

#endif  //WAYPOINT_FOLLWER_CLOTHOID_H_INCLUDED