#include <catch2/catch_test_macros.hpp>
#include "XMLParser.h"
#include <iostream>
#include <sstream>


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
    std::fstream ifs("../tests/testxml.txt", std::ios::in);
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
