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

#include "DFTraversal.h"

Define_Module(DFTraversal);

DFTraversal::DFTraversal()
    : status(UNVISITED)
    , initiator(false)
    , emisor(-1)
    , token(nullptr)
    , timer(nullptr){
    // TODO Auto-generated constructor stub
}


DFTraversal::~DFTraversal() {
    // TODO Auto-generated destructor stub
    cancelAndDelete(timer);
}

void DFTraversal::initialize() {
    // TODO Auto-generated destructor stub
    if(getIndex() == par("initiator").intValue()){
        omnetpp::simtime_t startTime = par("startTime");
        timer = new omnetpp::cMessage("timer", MsgKind::TIMER);
        scheduleAt(startTime, timer);
        token = new omnetpp::cMessage("token", MsgKind::TOKEN);
        status = Status::INITIATOR;
        initiator = true;
        EV_INFO << "Node[" << getIndex() << "] is the initiator \n";

    }
    //WATCH(unvisitedNeighbors)
    WATCH(status);
}

void DFTraversal::handleMessage(omnetpp::cMessage* recvMsg) {
    if(status == Status::INITIATOR){
        if(recvMsg->getKind() == MsgKind::TIMER){//A1
            unvisitedNeighbors.assign(gateSize("port$o"), true);
            initiator = true;
            visit(token);
        }else
            error("INVALID RULE at status = initiator\n");

    }
    else if(status == Status::UNVISITED){
        if(recvMsg->getKind() == MsgKind::TOKEN){
            emisor = recvMsg->getArrivalGate()->getIndex();
            unvisitedNeighbors.assign(gateSize("port$o"), true);
            unvisitedNeighbors[emisor] = 0;
            initiator = false;
            visit(recvMsg);
        }else
            error("INVALID RULE at status = unvisited\n");
    }
    else if(status == Status::VISITED){
        if(recvMsg->getKind() == MsgKind::TOKEN){
            int senderId = recvMsg->getArrivalGate()->getIndex();
            unvisitedNeighbors[senderId] = 0;
            recvMsg->setKind(MsgKind::BACKEDGE);
            recvMsg->setName("backedge");
            send(recvMsg, "port$o", senderId);
        }
        else
            visit(recvMsg);
    }
    else
        error("INVALID RULE at status = done\n");
}

 void DFTraversal::visit(omnetpp::cMessage* msg){
    int nextNeighbor = fFS();
    if(nextNeighbor != -1){
        unvisitedNeighbors[nextNeighbor] = 0;
        msg->setKind(MsgKind::TOKEN);
        msg->setName("token");
        send(msg, "port$o", nextNeighbor);
        status = Status::VISITED;
    }
    else{
        if(!initiator){
            msg->setKind(MsgKind::RETURN);
            msg->setName("return");
            send(msg, "port$o", emisor);
        }else
             delete msg;
        status = Status::DONE;
    }
}

 int DFTraversal::fFS(){
     int index = -1;
     for(size_t i = 0; i < unvisitedNeighbors.size(); i++){
         if (unvisitedNeighbors[i] == 1)
             index = i;
     }
     return index;
 }

void DFTraversal::refreshDisplay() const{
    std::string info;
    switch(status){
    case Status::INITIATOR:
        info = "initiator : ";
        break;
    case Status::UNVISITED:
        info = "unvisited : ";
        break;
    case Status::VISITED:
        info = "visited : ";
        break;
    case Status::DONE:
        info = "done : ";
        break;
    default:
        info = "unknown :";
    }
    for(size_t i = unvisitedNeighbors.size(); i> 0; i--){
            info+= std::to_string(unvisitedNeighbors[i-1]);
    }
    getDisplayString().setTagArg("t", 0, info.c_str());
}
