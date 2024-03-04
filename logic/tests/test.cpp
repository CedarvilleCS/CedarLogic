#include <catch2/catch_test_macros.hpp>
#include "XMLParser.h"
#include <iostream>
#include <sstream>
#include "logic_gate.h"
#include "logic_circuit.h"
#include "logic_event.h"
#include "logic_junction.h"
#include "logic_wire.h"

TEST_CASE("Logic event, [LogicEvent]") {

    SECTION("Logic event is initialized properly") {
        Event e1, e2;
        REQUIRE_FALSE(e1.isJunctionEvent);
        REQUIRE_FALSE(e1.newJunctionState);
        REQUIRE(e1.junctionID == ID_NONE);
        REQUIRE(e1.wireID == ID_NONE);
        REQUIRE(e1.gateID == ID_NONE);
        REQUIRE(e1.newState == UNKNOWN);
        REQUIRE(e1.eventTime == TIME_NONE);
        REQUIRE(e1.gateOutputID == "");
        REQUIRE(e1.getCreationTime() == 0);
        REQUIRE(e2.getCreationTime() == 1);
    }

    SECTION("Logic event comparison when simulation time differs") {
        Event e1, e2;
        e1.eventTime = 2;
        e2.eventTime = 1;
        REQUIRE(e1 > e2);
    }

    SECTION("When simulation time is the same then the object created later is greater") {
        Event e1, e2;
        e1.eventTime = 10;
        e2.eventTime = 10;
        REQUIRE(e2 > e1);
    }
}

TEST_CASE("Logic junction, [LogicJunction]") {

    SECTION("Logic junction defaults to enabled state") {
        Junction junc{123};
        REQUIRE(junc.getEnableState());
    }

    SECTION("Setting enable state works") {
        Junction junc{7};
        REQUIRE(junc.getEnableState());
        junc.setEnableState(false);
        REQUIRE_FALSE(junc.getEnableState());
        junc.setEnableState(true);
        REQUIRE(junc.getEnableState());
    }

    SECTION("GetWires returns an empty list for empty junction") {
        Junction junc{123};
        auto wires{junc.getWires()};
        REQUIRE(wires.empty());
    }

    SECTION("ConnectWire connects wire to junction") {
        Junction junc{123};
        junc.connectWire(17);
        junc.connectWire(42);
        auto wires{junc.getWires()};
        REQUIRE(wires.size() == 2);
        REQUIRE(wires.count(17) == 1);
        REQUIRE(wires.count(42) == 1);
    }

    SECTION("DisconnectWire returns true when the final instance of a wire is removed from the junction") {
        Junction junc{7};
        junc.connectWire(17);
        junc.connectWire(42);
        junc.connectWire(42);

        bool wasLastConnection = junc.disconnectWire(42);
        REQUIRE_FALSE(wasLastConnection);

        wasLastConnection = junc.disconnectWire(17);
        REQUIRE(wasLastConnection);

        // Causes log statement attempt to file which crashes test!
        // bool falseOnDisconnectedWire = junc.disconnectWire(17);
        // REQUIRE_FALSE(falseOnDisconnectedWire);

        auto wires{junc.getWires()};
        REQUIRE(wires.size() == 1);
        REQUIRE(wires.count(17) == 0);
        REQUIRE(wires.count(42) == 1);

        wasLastConnection = junc.disconnectWire(42);
        REQUIRE(wasLastConnection);
        auto wires2{junc.getWires()};
        REQUIRE(wires2.empty());
    }
}

TEST_CASE("Logic wire, [LogicWire]") {

    SECTION("Wires are created in high-impedance state") {
        Wire w;
        REQUIRE(w.getState() == HI_Z);
    }

    SECTION("WireInput operator < works primarily on gateID, secondarily on gateOutputID") {
        WireInput wi1{2, "a"};
        WireInput wi2{3, "b"};
        WireInput wi3{2, "b"};
        WireInput wi4{2, "a"};
        WireInput wi5{1, "b"};
        
        REQUIRE(wi1 < wi2);
        REQUIRE(wi5 < wi4);
        REQUIRE(wi3 < wi2);
        REQUIRE_FALSE(wi4 < wi5);
        REQUIRE_FALSE(wi2 < wi1);
        REQUIRE_FALSE(wi2 < wi3);
        REQUIRE(wi1 < wi3);
        REQUIRE_FALSE(wi3 < wi1);
        REQUIRE_FALSE(wi1 < wi1);
    }

    SECTION("WireOutput operator < works primarily on gateID, secondarily on gateInputID") {
        WireOutput wo1{2, "a"};
        WireOutput wo2{3, "b"};
        WireOutput wo3{2, "b"};
        WireOutput wo4{2, "a"};
        WireOutput wo5{1, "b"};
        
        REQUIRE(wo1 < wo2);
        REQUIRE(wo5 < wo4);
        REQUIRE(wo3 < wo2);
        REQUIRE_FALSE(wo4 < wo5);
        REQUIRE_FALSE(wo2 < wo1);
        REQUIRE_FALSE(wo2 < wo3);
        REQUIRE(wo1 < wo3);
        REQUIRE_FALSE(wo3 < wo1);
        REQUIRE_FALSE(wo1 < wo1);
    }

    // TODO: WIRE_PTR operator <

    SECTION("Wire can be forced to a given state") {
        // !StateType is just unsigned char so values 5->
        // are theoretically possible...

        // Range of values:
        // const StateType ZERO = 0;
        // const StateType ONE = 1;
        // const StateType HI_Z = 2;
        // const StateType CONFLICT = 3;
        // const StateType UNKNOWN = 4;

        Wire w;
        REQUIRE(w.getState() == HI_Z);
        w.forceState(ZERO);
        REQUIRE(w.getState() == ZERO);
        w.forceState(ONE);
        REQUIRE(w.getState() == ONE);
        w.forceState(CONFLICT);
        REQUIRE(w.getState() == CONFLICT);
        w.forceState(UNKNOWN);
        REQUIRE(w.getState() == UNKNOWN);
        w.forceState(HI_Z);
        REQUIRE(w.getState() == HI_Z);
    }

    SECTION("Wire getFirstInput returns first (sorted) non-NONE input or junk") {

        Wire w, w2;
        REQUIRE(w.getFirstInput().gateID == ID_NONE);
        w.connectInput(7, "bb");
        w.connectInput(3, "a");
        REQUIRE(w.getFirstInput().gateID == 3);

        w2.connectInput(ID_NONE, "asd");
        auto wi2 = w2.getFirstInput();
        REQUIRE(((wi2.gateID == ID_NONE) && (wi2.gateOutputID == "")));
    }

    SECTION("Wire getFirstOutput returns first (sorted) output or junk") {
        Wire w;
        REQUIRE(w.getFirstOutput().gateID == ID_NONE);
        w.connectOutput(8, "some-gate");
        w.connectOutput(5, "other-gate");
        REQUIRE(w.getFirstOutput().gateID == 5);
    }

    SECTION("Wire getOutputGates") {
        Wire w;
        REQUIRE(w.getOutputGates().empty());
        w.connectOutput(55, "some-gate");
        w.connectOutput(78, "other-gate");
        auto gates = w.getOutputGates();
        REQUIRE(gates.size() == 2);
        REQUIRE(gates.front() == 55);
        REQUIRE(gates.back() == 78);
    }
}

TEST_CASE("Logic gate setParameter during construction, [LogicGate]") {
    
    SECTION("N_INPUT") {
        Gate_N_INPUT ngate;
        REQUIRE(ngate.getParameter("INPUT_BITS") == "0");
    }

    SECTION("COMPARE") {
        Gate_COMPARE cgate;
        REQUIRE(cgate.getParameter("INPUT_BITS") == "0");
    }
    
    SECTION("GATE PASS") {
        Gate_PASS pgate;
        REQUIRE(pgate.getParameter("INPUT_BITS") == "1");
    }

    SECTION("GATE MUX") {
        Gate_MUX mgate;
        REQUIRE(mgate.getParameter("INPUT_BITS") == "0");
    }

    SECTION("GATE DRIVER") {
        Gate_DRIVER driver_gate;
        REQUIRE(driver_gate.getParameter("OUTPUT_BITS") == "0");
    }

    SECTION("GATE DECODER") {
        Gate_DECODER decoder_gate;
        REQUIRE(decoder_gate.getParameter("INPUT_BITS") == "0");
    }

    SECTION("GATE PRI ENCODER") {
        Gate_PRI_ENCODER encoder_gate;
        REQUIRE(encoder_gate.getParameter("INPUT_BITS") == "0");
    }

    SECTION("GATE ADDER") {
        Gate_ADDER adder_gate;
        REQUIRE(adder_gate.getParameter("INPUT_BITS") == "0");
    }

    SECTION("GATE JFKK") {
        Gate_JKFF JFKK_gate;
        REQUIRE(JFKK_gate.getParameter("SYNC_SET") == "false");
        REQUIRE(JFKK_gate.getParameter("SYNC_CLEAR") == "false");
    }

    SECTION("GATE RAM") {
        Gate_RAM RAM_gate;
        REQUIRE(RAM_gate.getParameter("ADDRESS_BITS") == "0");
        REQUIRE(RAM_gate.getParameter("DATA_BITS") == "0");
    }

    SECTION("GATE Junction") {
        Circuit cir;
        Gate_JUNCTION junction_gate(&cir);
        REQUIRE(junction_gate.getParameter("JUNCTION_ID") == "NONE");
    }
}

TEST_CASE("XMLParser writing, [XMLParser]") {
    std::ostringstream oss;
    XMLParser parser(&oss);

    SECTION("OpenTag") {
        parser.openTag("myTag");
        REQUIRE(oss.str() == "<myTag>");
    }

    SECTION("When opening more than one tag Then the tags are on separate lines") {
        parser.openTag("myTag");
        parser.openTag("mySecondTag");
        REQUIRE(oss.str() == "<myTag>\n<mySecondTag>");
    }

    SECTION("Create open tag with empty tag name") {
        parser.openTag("");
        REQUIRE(oss.str() == "<>");
    }

    SECTION("Write value for open tag") {
        parser.openTag("myTag");
        parser.writeTag("myTag", "myVal");
        REQUIRE(oss.str() == "<myTag>myVal");
    }

    SECTION("Write multiple values for open tag") {
        parser.openTag("myTag");
        parser.writeTag("myTag", "myVal");
        parser.writeTag("myTag", "myOtherVal");
        parser.writeTag("myTag", "myThirdVal");
        REQUIRE(oss.str() == "<myTag>myValmyOtherValmyThirdVal");
    }

    SECTION("Write empty value for open tag") {
        parser.openTag("myTag");
        parser.writeTag("myTag", "");
        REQUIRE(oss.str() == "<myTag>");
    }

    SECTION("Write value for open tag that is not the latest writes nothing") {
        parser.openTag("otherTag");
        parser.openTag("myTag");
        parser.writeTag("otherTag", "otherVal");
        REQUIRE(oss.str() == "<otherTag>\n<myTag>");
    }

    SECTION("Write value for open tag that doesn't exist writes nothing") {
        parser.openTag("myTag");
        parser.writeTag("otherTag", "otherVal");
        REQUIRE(oss.str() == "<myTag>");
    }

    SECTION("Values with < in the beginning get the character replaced with bell") {
        parser.openTag("myTag");
        parser.writeTag("myTag", "<<weirdValue>");
        REQUIRE(oss.str() == "<myTag>\a\aweirdValue>");
    }

    SECTION("closeTag with null string writes nothing") {
        parser.openTag("myTag");
        parser.writeTag("myTag", "myVal");
        parser.closeTag("");
        REQUIRE(oss.str() == "<myTag>myVal");
    }

    SECTION("CloseTag closes latest open tag") {
        parser.openTag("myTag");
        parser.writeTag("myTag", "myVal");
        parser.closeTag("myTag");
        REQUIRE(oss.str() == "<myTag>myVal</myTag>");;
    }

    SECTION("CloseTag with not the latest open tag writes nothing") {
        parser.openTag("firstTag");
        parser.openTag("myTag");
        parser.writeTag("myTag", "myVal");
        parser.closeTag("firstTag");
        REQUIRE(oss.str() == "<firstTag>\n<myTag>myVal");
    }

    SECTION("CloseTag with non-existent tag writes nothing") {
        parser.openTag("myTag");
        parser.writeTag("myTag", "myVal");
        parser.closeTag("nonsense");
        REQUIRE(oss.str() == "<myTag>myVal");
    }
}

TEST_CASE("XMLParser reading, [XMLParser]") {
    std::fstream ifs("../tests/testxml.txt", std::ios::in);  // TODO: get this test file auto-copied over
    XMLParser parser(&ifs, false);

    if (ifs.is_open())
    {
        std::cout << "File successfully open\n";
    }
    else
    {
        std::cout << "File not opened!\n";
    }
	
    // Contents of test-file:
    // <hello>HelloValue</hello>
    // <port>PortValue
    // <inner-port>InnerValue</inner-port></port>
    //# Comment
    //<final>Final</final>

    SECTION("getCurrentIdx returns zero on construction") {
        REQUIRE(parser.getCurrentIndex() == 0);
    }

    SECTION("ReadTag") {
        REQUIRE(parser.readTag() == "hello");
    }

    SECTION("ReadTagValue") {
        REQUIRE(parser.readTag() == "hello");
        REQUIRE(parser.readTagValue("hello") == "HelloValue");
    }

    SECTION("ReadTagValue on non-value token returns empty string") {
        REQUIRE(parser.readTagValue("hello") == "");
    }

    SECTION("ReadTagValue with arbitrary tagName returns current tag value") {
        REQUIRE(parser.readTag() == "hello");
        REQUIRE(parser.readTagValue("nonsense") == "HelloValue");
    }

    SECTION("ReadTag repeatedly returns empty string until readCloseTag is called") {
        REQUIRE(parser.readTag() == "hello");
        REQUIRE(parser.readTag() == "");
        REQUIRE(parser.readTag() == "");
        REQUIRE(parser.readTag() == "");
        REQUIRE(parser.readCloseTag() == "hello");
        REQUIRE(parser.readTag() == "port");
    }

    SECTION("ReadCloseTag closes the most open tag") {
        REQUIRE(parser.readTag() == "hello");
        REQUIRE(parser.readCloseTag() == "hello");
        REQUIRE(parser.readTag() == "port");
        REQUIRE(parser.readTag() == "inner-port");
        REQUIRE(parser.readCloseTag() == "inner-port");
        REQUIRE(parser.readCloseTag() == "port");
    }

    SECTION("ReadCloseTag reads ahead tokens even if they are not read with readTag") {
        REQUIRE(parser.readCloseTag() == "hello");
    }

    SECTION("isTag and isCloseTag check the next token ahead") {
        REQUIRE(parser.isTag(parser.getCurrentIndex()) == true);
        REQUIRE(parser.readTag() == "hello");
        REQUIRE(parser.isTag(parser.getCurrentIndex()) == false);
        REQUIRE(parser.isCloseTag(parser.getCurrentIndex()) == false);
        parser.readTagValue("hello");
        REQUIRE(parser.isTag(parser.getCurrentIndex()) == false);
        REQUIRE(parser.isCloseTag(parser.getCurrentIndex()) == true);
        REQUIRE(parser.readCloseTag() == "hello");
        REQUIRE(parser.isTag(parser.getCurrentIndex()) == true);
        REQUIRE(parser.isCloseTag(parser.getCurrentIndex()) == false);
        REQUIRE(parser.readTag() == "port");
        REQUIRE(parser.isTag(parser.getCurrentIndex()) == false);
        REQUIRE(parser.isCloseTag(parser.getCurrentIndex()) == false);
        REQUIRE(parser.readTag() == "inner-port");
        REQUIRE(parser.readCloseTag() == "inner-port");
        REQUIRE(parser.isTag(parser.getCurrentIndex()) == false);
        REQUIRE(parser.isCloseTag(parser.getCurrentIndex()) == true);
        REQUIRE(parser.readCloseTag() == "port");
    }

    SECTION("ReadTag munches comments and returns next token data") {
        REQUIRE(parser.readTag() == "hello");
        REQUIRE(parser.readCloseTag() == "hello");
        REQUIRE(parser.readTag() == "port");
        REQUIRE(parser.readTag() == "inner-port");
        REQUIRE(parser.readCloseTag() == "inner-port");
        REQUIRE(parser.readCloseTag() == "port");
        REQUIRE(parser.readTag() == "final");
        REQUIRE(parser.readTagValue("final") == "Final");
        REQUIRE(parser.readCloseTag() == "final");
    }
}
