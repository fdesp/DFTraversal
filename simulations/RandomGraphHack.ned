//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

package dftraversal.simulations;
import dftraversal.DFTraversalHack;

channel Link2 extends ned.DatarateChannel {
    parameters:
        int cost = default(0);
        // delay = exponential(1s);
        delay = 1s;
}

//
// Generated network with random topology (5 nodes, 7 edges, seed=0).
//
network RandomGraphHack {
    parameters:
        @display("bgb=300,300");
        int initiator;
        node[*].initiator = initiator;
       
    submodules:
		node[5]: DFTraversalHack;
    connections:
        node[0].port++ <--> Link2 <--> node[1].port++;
        node[0].port++ <--> Link2 <--> node[2].port++;
        node[0].port++ <--> Link2 <--> node[4].port++;
        node[1].port++ <--> Link2 <--> node[2].port++;
        node[1].port++ <--> Link2 <--> node[3].port++;
        node[2].port++ <--> Link2 <--> node[3].port++;
        node[2].port++ <--> Link2 <--> node[4].port++;
}
