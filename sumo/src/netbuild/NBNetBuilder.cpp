#include <string>
#include <fstream>
#include <iostream>
#include <iomanip>
#include "NBNetBuilder.h"
#include "nodes/NBNodeCont.h"
#include "NBEdgeCont.h"
#include "NBTrafficLightLogicCont.h"
#include "NBJunctionLogicCont.h"
#include "NBDistrictCont.h"
#include "NBDistribution.h"
#include "NBRequest.h"
#include "NBTypeCont.h"
#include <utils/options/OptionsCont.h>
#include <utils/options/OptionsSubSys.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/StringTokenizer.h>
#include <utils/convert/ToString.h>
#include "NBJoinedEdgesMap.h"

using namespace std;

NBNetBuilder::NBNetBuilder()
{
}


NBNetBuilder::~NBNetBuilder()
{
}


void
NBNetBuilder::buildLoaded()
{
    NBTypeCont::report();
    NBEdgeCont::report();
    NBNodeCont::report();
    // perform the computation
    OptionsCont &oc = OptionsSubSys::getOptions();
    compute(oc);
    // save network when wished
    save(oc.getString("o"), oc);
    // save plain nodes/edges/connections
    if(oc.isSet("plain-output")) {
        savePlain(oc.getString("plain-output"));
    }
    // save the mapping information when wished
    if(oc.isSet("map-output")) {
        saveMap(oc.getString("map-output"));
    }
}


void
NBNetBuilder::inform(int &step, const std::string &about)
{
    WRITE_MESSAGE(string("Computing step ") + toString<int>(step)+ string(": ") + about);
    step++;
}


bool
NBNetBuilder::removeDummyEdges(int &step)
{
    // Removes edges that are connecting the same node
    inform(step, "Removing dummy edges ");
    return NBNodeCont::removeDummyEdges();
}


bool
NBNetBuilder::joinEdges(int &step)
{
    inform(step, "Joining double connections");
    return NBNodeCont::recheckEdges();
}


bool
NBNetBuilder::removeUnwishedNodes(int &step, OptionsCont &oc)
{
    if(oc.getBool("no-node-removal")) {
        return true;
    }
    inform(step, "Removing empty nodes and geometry nodes.");
    return NBNodeCont::removeUnwishedNodes();
}


bool
NBNetBuilder::removeUnwishedEdges(int &step, OptionsCont &oc)
{
    if(!OptionsSubSys::getOptions().isSet("keep-edges")) {
        return true;
    }
    inform(step, "Removing unwished edges.");
    return NBEdgeCont::removeUnwishedEdges(oc);
}


bool
NBNetBuilder::guessTLs(int &step, OptionsCont &oc)
{
    inform(step, "Guessing and setting TLs");
    if(oc.isSet("explicite-tls")) {
        StringTokenizer st(oc.getString("explicite-tls"), ";");
        while(st.hasNext()) {
            NBNodeCont::setAsTLControlled(st.next());
        }
    }
    return NBNodeCont::guessTLs(oc);
}


bool
NBNetBuilder::computeTurningDirections(int &step)
{
    inform(step, "Computing turning directions");
    return NBEdgeCont::computeTurningDirections();
}


bool
NBNetBuilder::sortNodesEdges(int &step)
{
    inform(step, "Sorting nodes' edges");
    return NBNodeCont::sortNodesEdges();
}


bool
NBNetBuilder::normaliseNodePositions(int &step)
{
    inform(step, "Normalising node positions");
    bool ok = NBNodeCont::normaliseNodePositions();
    if(ok) {
        ok = NBEdgeCont::normaliseEdgePositions();
    }
    return ok;
}


bool
NBNetBuilder::computeEdge2Edges(int &step)
{
    inform(step, "Computing Approached Edges");
    return NBEdgeCont::computeEdge2Edges();
}


bool
NBNetBuilder::computeLanes2Edges(int &step)
{
    inform(step, "Computing Approaching Lanes");
    return NBEdgeCont::computeLanes2Edges();
}


bool
NBNetBuilder::computeLanes2Lanes(int &step)
{
    inform(step, "Dividing of Lanes on Approached Lanes");
    bool ok = NBNodeCont::computeLanes2Lanes();
    if(ok) {
        return NBEdgeCont::sortOutgoingLanesConnections();
    }
    return ok;
}

bool
NBNetBuilder::recheckLanes(int &step)
{
    inform(step, "Rechecking of lane endings");
    return NBEdgeCont::recheckLanes();
}


bool
NBNetBuilder::computeNodeShapes(int &step)
{
    inform(step, "Computing node shapes");
    return NBNodeCont::computeNodeShapes();
}


bool
NBNetBuilder::computeEdgeShapes(int &step)
{
    inform(step, "Computing edge shapes");
    return NBEdgeCont::computeEdgeShapes();
}


/** computes the node-internal priorities of links */
/*bool
computeLinkPriorities(int step, bool verbose)
{
    if(verbose) {
        cout << "Computing step " << step
            << ": Computing Link Priorities" << endl;
    }
    return NBEdgeCont::computeLinkPriorities(verbose);
}
*/

bool
NBNetBuilder::appendTurnarounds(int &step, OptionsCont &oc)
{
    if(!oc.getBool("append-turnarounds")) {
        return true;
    }
    inform(step, "Appending Turnarounds");
    return NBEdgeCont::appendTurnarounds();
}


bool
NBNetBuilder::setTLControllingInformation(int &step)
{
    inform(step, "Computing node logics");
    return NBTrafficLightLogicCont::setTLControllingInformation();
}


bool
NBNetBuilder::computeLogic(int &step, OptionsCont &oc)
{
    inform(step, "Computing node logics");
    return NBNodeCont::computeLogics(oc);
}


bool
NBNetBuilder::computeTLLogic(int &step, OptionsCont &oc)
{
    inform(step, "Computing traffic light logics");
    return NBTrafficLightLogicCont::computeLogics(oc);
}


bool
NBNetBuilder::reshiftRotateNet(int &step, OptionsCont &oc)
{
    if(oc.isDefault("x-offset-to-apply")) {
        return true;
    }
    inform(step, "Transposing network");
    double xoff = oc.getFloat("x-offset-to-apply");
    double yoff = oc.getFloat("y-offset-to-apply");
    double rot = oc.getFloat("rotation-to-apply");
    inform(step, "Normalising node positions");
    bool ok = NBNodeCont::reshiftNodePositions(xoff, yoff, rot);
    if(ok) {
        ok = NBEdgeCont::reshiftEdgePositions(xoff, yoff, rot);
    }
    return ok;
}




void
NBNetBuilder::compute(OptionsCont &oc)
{
    bool ok = true;
    int step = 1;
//    if(ok) ok = setInit(step++);
    //
    if(ok) ok = removeDummyEdges(step);
    gJoinedEdges.init(NBEdgeCont::getAllNames());
    if(ok) ok = joinEdges(step);
    if(ok) ok = removeUnwishedNodes(step, oc);
    if(ok&&oc.getBool("keep-edges.postload")) {
        if(ok) ok = removeUnwishedEdges(step, oc);
        if(ok) ok = removeUnwishedNodes(step, oc);
    }
    if(ok) ok = guessTLs(step, oc);
    if(ok) ok = computeTurningDirections(step);
    if(ok) ok = sortNodesEdges(step);
    if(ok) ok = normaliseNodePositions(step);
    if(ok) ok = computeEdge2Edges(step);
    if(ok) ok = computeLanes2Edges(step);
    if(ok) ok = computeLanes2Lanes(step);
    if(ok) ok = appendTurnarounds(step, oc);
    if(ok) ok = recheckLanes(step);
    if(ok) ok = computeNodeShapes(step);
    if(ok) ok = computeEdgeShapes(step);
//    if(ok) ok = computeLinkPriorities(step++);
    if(ok) ok = setTLControllingInformation(step);
    if(ok) ok = computeLogic(step, oc);
    if(ok) ok = computeTLLogic(step, oc);

    if(ok) ok = reshiftRotateNet(step, oc);

    NBNode::reportBuild();
    NBRequest::reportWarnings();
    checkPrint(oc);
    if(!ok) {
        throw ProcessError();
    }
}


void
NBNetBuilder::checkPrint(OptionsCont &oc)
{
    if(oc.getBool("print-node-positions")) {
        NBNodeCont::printNodePositions();
    }
}


bool
NBNetBuilder::save(string path, OptionsCont &oc)
{
    // try to build the output file
    ofstream res(path.c_str());
    if(!res.good()) {
        return false;
    }
    // print the computed values
    res << "<net>" << endl << endl;
    res.setf( ios::fixed, ios::floatfield );
    res << setprecision( 2 );
    // write the numbers of some elements
    std::vector<std::string> ids;
    if(oc.getBool("add-internal-links")) {
        ids = NBNodeCont::getInternalNamesList();
    }
    NBEdgeCont::writeXMLEdgeList(res, ids);
    if(oc.getBool("add-internal-links")) {
        NBNodeCont::writeXMLInternalLinks(res);
    }

        // write the number of nodes
    NBNodeCont::writeXMLNumber(res);
    res << endl;
    // write the districts
    NBDistrictCont::writeXML(res);
    // write edges with lanes and connected edges
    NBEdgeCont::writeXMLStep1(res);
    // write the logics
    NBJunctionLogicCont::writeXML(res);
    NBTrafficLightLogicCont::writeXML(res);
    // write the nodes
    NBNodeCont::writeXML(res);
    // write the successors of lanes
    NBEdgeCont::writeXMLStep2(res);
    if(oc.getBool("add-internal-links")) {
        NBNodeCont::writeXMLInternalSuccInfos(res);
    }
    // write the positions of edges
    NBEdgeCont::writeXMLStep3(res);
    if(oc.getBool("add-internal-links")) {
        NBNodeCont::writeXMLInternalEdgePos(res);
    }
    res << "</net>" << endl;
    return true;
}


void
NBNetBuilder::savePlain(const std::string &filename)
{
    // try to build the output file
    NBNodeCont::savePlain(filename + string(".nod.xml"));
    NBEdgeCont::savePlain(filename + string(".edg.xml"));
//    NBConnectionCont::svaePlain(filename);
}


bool
NBNetBuilder::saveMap(string path)
{
    // try to build the output file
    ofstream res(path.c_str());
    if(!res.good()) {
        return false;
    }
    // write map
    res << gJoinedEdges;
    return true;
}


void
NBNetBuilder::insertNetBuildOptions(OptionsCont &oc)
{
    // register building defaults
    oc.doRegister("type", 'T', new Option_String("Unknown"));
    oc.doRegister("lanenumber", 'L', new Option_Integer(1));
    oc.doRegister("speed", 'S', new Option_Float((float) 13.9));
    oc.doRegister("priority", 'P', new Option_Integer(1));
    // register computation variables
    oc.doRegister("min-decel", 'D', new Option_Float(3.0));
    // register the report options
    oc.doRegister("verbose", 'v', new Option_Bool(false));
    oc.doRegister("suppress-warnings", 'W', new Option_Bool(false));
    oc.doRegister("print-options", 'p', new Option_Bool(false));
    oc.doRegister("help", new Option_Bool(false));
    oc.doRegister("log-file", 'l', new Option_FileName());
    // extended
    oc.doRegister("print-node-positions", new Option_Bool(false));
    // register the data processing options
    oc.doRegister("recompute-junction-logics", new Option_Bool(false));
    oc.doRegister("omit-corrupt-edges", new Option_Bool(false));
    oc.doRegister("flip-y", new Option_Bool(false));
    oc.doRegister("all-logics", new Option_Bool(false));
    oc.doRegister("use-laneno-as-priority", new Option_Bool(false));
    oc.doRegister("keep-small-tyellow", new Option_Bool(false));
    oc.doRegister("traffic-light-green", new Option_Integer());
    oc.doRegister("traffic-light-yellow", new Option_Integer());
    oc.doRegister("x-offset-to-apply", new Option_Float(0));
    oc.doRegister("y-offset-to-apply", new Option_Float(0));
    oc.doRegister("rotation-to-apply", new Option_Float(0));
    oc.doRegister("no-node-removal", new Option_Bool(false));
    oc.doRegister("append-turnarounds", new Option_Bool(false));
    oc.doRegister("add-internal-links", 'I', new Option_Bool(false));
    oc.doRegister("keep-unregulated", new Option_Bool(false));

    oc.doRegister("map-output", 'M', new Option_FileName());

    // tls-guessing
    oc.doRegister("guess-tls", new Option_Bool(false));
    oc.doRegister("tls-guess.no-incoming-min", new Option_Integer(2));
    oc.doRegister("tls-guess.no-incoming-max", new Option_Integer(5));
    oc.doRegister("tls-guess.no-outgoing-min", new Option_Integer(1));
    oc.doRegister("tls-guess.no-outgoing-max", new Option_Integer(5));
    oc.doRegister("tls-guess.min-incoming-speed", new Option_Float((float) (40/3.6)));
    oc.doRegister("tls-guess.max-incoming-speed", new Option_Float((float)(69/3.6)));
    oc.doRegister("tls-guess.min-outgoing-speed", new Option_Float((float)(40/3.6)));
    oc.doRegister("tls-guess.max-outgoing-speed", new Option_Float((float)(69/3.6)));

    oc.doRegister("explicite-tls", new Option_String());
    oc.doRegister("explicite-no-tls", new Option_String());
    oc.doRegister("edges-min-speed", new Option_Float());
    oc.doRegister("keep-edges", new Option_String());
    oc.doRegister("keep-edges.input-file", new Option_FileName());
    oc.doRegister("keep-edges.postload", new Option_Bool(false));

    oc.doRegister("plain-output", new Option_FileName());

    oc.doRegister("netbuild.debug", new Option_Integer(0));

}



void
NBNetBuilder::preCheckOptions(OptionsCont &oc)
{
    // we possibly have to load the edges to keep
    if(oc.isSet("keep-edges.input-file")) {
        ifstream strm(oc.getString("keep-edges.input-file").c_str());
        if(!strm.good()) {
            MsgHandler::getErrorInstance()->inform(
                "Could not load names of edges too keep from '"
                + oc.getString("keep-edges.input-file") + "'.");
            throw ProcessError();
        }
        std::ostringstream oss;
        bool first = true;
        while(strm.good()) {
            if(!first) {
                oss << ';';
            }
            string name;
            strm >> name;
            oss << name;
            first = false;
        }
        oc.set("keep-edges", oss.str());
    }
}


