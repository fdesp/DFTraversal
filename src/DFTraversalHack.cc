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

#include "DFTraversalHack.h"

Define_Module(DFTraversalHack);


DFTraversalHack::DFTraversalHack()
    : status(IDLE)
    , initiator(false)
    , emisor(-1)
    , token(nullptr)
    , timer(nullptr){
// TODO Auto-generated constructor stub
}


DFTraversalHack::~DFTraversalHack() {
    // TODO Auto-generated destructor stub
    cancelAndDelete(timer);
}

void DFTraversalHack::initialize() {
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


void DFTraversalHack::handleMessage(omnetpp::cMessage* recvMsg){
    if(status == Status::INITIATOR){
        if(recvMsg->getKind() == MsgKind::TIMER){//A1
            unvisitedNeighbors.assign(gateSize("port$o"), true);
            int nextNeighbor = fFS();
            unvisitedNeighbors[nextNeighbor] = 0;
            send(token, "port$o", nextNeighbor);
            recvMsg->setKind(MsgKind::MVISITED);
            recvMsg->setName("visited");
            int neighborhoodSize = gateSize("port$o");
            for (int i = 0; i < neighborhoodSize; i++)
                if(i != nextNeighbor)
                    send(recvMsg->dup(), "port$o", i);
            status = Status::SVISITED;
        }else
            error("INVALID RULE at status = initiator\n");

    }else if(status == Status::IDLE){
        if(recvMsg->getKind() == MsgKind::TOKEN){//A2
            unvisitedNeighbors.assign(gateSize("port$o"), true);
            firstVisit(recvMsg);
        }else if(recvMsg->getKind() == MsgKind::MVISITED){
            unvisitedNeighbors.assign((gateSize("port$o")), true);
            emisor = recvMsg->getArrivalGate()->getIndex();
            unvisitedNeighbors[emisor] = 0;
            status = Status::AVAILABLE;
            delete recvMsg;
        }
        else
            error("INVALID RULE at status = idle\n");

    }else if(status == Status::AVAILABLE){
        if(recvMsg->getKind() == MsgKind::TOKEN){ //A3
            firstVisit(recvMsg);
        }

        if(recvMsg->getKind() == MsgKind::MVISITED){
            emisor = recvMsg->getArrivalGate()->getIndex();
            unvisitedNeighbors[emisor] = 0;
            delete recvMsg;
        }
        else
            error("INVALID RULE at status = available\n");

    }else if(status == Status::SVISITED){
        if(recvMsg->getKind() == MsgKind::MVISITED){//A4
            emisor = recvMsg->getArrivalGate()->getIndex();
            unvisitedNeighbors[emisor] = 0;
            int nextNeighbor = fFS();
            if(nextNeighbor == emisor){
                visit(recvMsg);
            }
            delete recvMsg;
        }else if(recvMsg->getKind() == MsgKind::TOKEN){
            emisor = recvMsg->getArrivalGate()->getIndex();
            unvisitedNeighbors[emisor] = 0;
            int nextNeighbor = fFS();
            if(nextNeighbor == emisor){
                visit(recvMsg);
            }
        }else if(recvMsg->getKind() == MsgKind::RETURN){
            visit(recvMsg);
        }else
            error("INVALID RULE at status = visited\n");
    }
}

void DFTraversalHack::firstVisit(omnetpp::cMessage* recvMsg){
    initiator = false;
    entry = recvMsg->getArrivalGate()->getIndex();
    unvisitedNeighbors[entry] = 0;

    int nextNeighbor = fFS();
    if(nextNeighbor != -1){
        unvisitedNeighbors[nextNeighbor] = 0;
        recvMsg->setKind(MsgKind::TOKEN);
        recvMsg->setName("token");
        send(recvMsg, "port$o", nextNeighbor);
        recvMsg->setKind(MsgKind::MVISITED);
        recvMsg->setName("visited");
        int neighborhoodSize = gateSize("port$o");
        for (int i = 0; i < neighborhoodSize; i++)
            if(i != nextNeighbor || i != entry)
                send(recvMsg->dup(), "port$o", i);
        status = Status::SVISITED;
    }
    else{
        recvMsg->setKind(MsgKind::RETURN);
        recvMsg->setName("return");
        send(recvMsg, "port$o", entry);
        recvMsg->setKind(MsgKind::MVISITED);
        recvMsg->setName("visited");
        int neighborhoodSize = gateSize("port$o");
        for (int i = 0; i < neighborhoodSize; i++)
            if(i != entry)
                send(recvMsg->dup(), "port$o", i);
        status = Status::DONE;
    }
}

void DFTraversalHack::visit(omnetpp::cMessage* recvMsg){
    int nextNeighbor = fFS();
    if(nextNeighbor != -1){
        unvisitedNeighbors[emisor] = 0;
        recvMsg->setKind(MsgKind::TOKEN);
        recvMsg->setName("token");
        send(recvMsg, "port$o", nextNeighbor);
    }else{
        if(!initiator){
            recvMsg->setKind(MsgKind::RETURN);
            recvMsg->setName("return");
            send(recvMsg, "port$o", entry);
        }
        status = Status::DONE;
    }
}

int DFTraversalHack::fFS(){
    int index = -1;
    for(size_t i = 0; i < unvisitedNeighbors.size(); i++){
        if (unvisitedNeighbors[i] == 1)
            index = i;
    }
    return index;
}

void DFTraversalHack::refreshDisplay() const{
    std::string info;
    switch(status){
    case Status::INITIATOR:
        info = "initiator : ";
        break;
    case Status::IDLE:
        info = "idle : ";
        break;
    case Status::AVAILABLE:
        info = "available : ";
        break;
    case Status::SVISITED:
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


