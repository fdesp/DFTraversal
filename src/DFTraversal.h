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

#ifndef DFTRAVERSAL_H_
#define DFTRAVERSAL_H_

#include <omnetpp.h>
#include <vector>
#include <string>


class DFTraversal: public omnetpp::cSimpleModule {


public:
    enum Status{
        INITIATOR = 0,
        UNVISITED,
        VISITED,
        DONE
    };
    enum MsgKind{
        TOKEN = 10,
        BACKEDGE,
        RETURN,
        TIMER
    };

    /** @brief The node status */
    Status status;
    /** @brief Register indicating this node starts as initiator or not*/
    bool initiator;
    /* @brief The port number at which the TOKEN was received at first time */
    int emisor;
    /* @brief The only-one message traversing the network */
    omnetpp ::cMessage* token;
    /* @brief The timer that rings when the simulation starts*/
    omnetpp ::cMessage* timer;
    /* @brief The set of unvisited */
    std::vector<bool> unvisitedNeighbors;

protected:
    /** @brief Prints the node starts in the simulation canvas */
    virtual void refreshDisplay() const override;

    /** @brief Procedure to search into the neighborhood of this node */
    virtual void visit(omnetpp::cMessage*);
    /** @ Finds the first bit set in one. */
    /** @ return the position of the FFS bit it is found, otherwise -1 */
    virtual int fFS();
public:
    /** @brief Default constructor */
    DFTraversal();

    /** @ brief Default destructor */
    virtual ~DFTraversal();

    /** @brief Perform the parameters of this node */
    virtual void initialize();

    /* @brief Performs the set of actions of this DFTraversal protocol */
    virtual void handleMessage(omnetpp::cMessage*);
};


#endif /* DFTRAVERSAL_H_ */
