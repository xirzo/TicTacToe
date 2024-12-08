#include <gtest/gtest.h>

#include <sstream>

class ParserTest : public ::testing::Test {
protected:
    // std::unique_ptr<Game> game;
    // std::unique_ptr<CommandsHandler> handler;
    // std::unique_ptr<Parser> parser;

    void SetUp() override {
        // game = std::make_unique<Game>();
        // handler = std::make_unique<CommandsHandler>(*game);
        // parser = std::make_unique<Parser>(*handler);

        original_cin_buf = std::cin.rdbuf();
        original_cout_buf = std::cout.rdbuf();

        input_stream = std::make_unique<std::stringstream>();

        output_stream = std::make_unique<std::stringstream>();

        std::cin.rdbuf(input_stream->rdbuf());
        std::cout.rdbuf(output_stream->rdbuf());
    }

    void TearDown() override {
        std::cin.rdbuf(original_cin_buf);
        std::cout.rdbuf(original_cout_buf);
    }

    std::streambuf* original_cin_buf;
    std::streambuf* original_cout_buf;

    std::unique_ptr<std::stringstream> input_stream;
    std::unique_ptr<std::stringstream> output_stream;
};

TEST_F(ParserTest, Invalid) {
    *input_stream << "invalid" << std::endl;

    // parser->GetInput();

    std::stringstream expected;
    expected << "Error: Invalid command!" << std::endl;

    EXPECT_EQ(output_stream->str(), expected.str());
}
