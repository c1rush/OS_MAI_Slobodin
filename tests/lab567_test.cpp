#include <gtest/gtest.h>
#include <set>
#include "tree.hpp"
#include "manage_zmq.hpp"

// export PROGRAM_PATH="$(pwd)/lab5-7/server"

TEST(Tests, PingTest) {
    std::string programPath = getenv("PROGRAM_PATH");
    std::set<int> Nodes;
    Node task(-1);
    Nodes.insert(-1);
    std::string ans = task.Create(1, programPath);
    Nodes.insert(1);

    ans = task.Send("ping 1", 1);
    EXPECT_EQ(ans, "Ok:1");
    ans = task.Send("ping 2", 2);
    EXPECT_EQ(ans, "Error: Not found");

    task.Kill();
}

TEST(Tests, ExecTest_Found) {
    std::string programPath = getenv("PROGRAM_PATH");
    std::set<int> Nodes;
    Node task(-1);
    Nodes.insert(-1);
    std::string ans = task.Create(1, programPath);
    Nodes.insert(1);

    // Отправка команды exec 1|abracadabra|abra
    std::string execCommand = "exec 1|abracadabra|abra";
    ans = task.Send(execCommand, 1);
    EXPECT_EQ(ans, "Ok:1:0;7");
    task.Kill();
}

TEST(Tests, ExecTest_NotFound) {
    std::string programPath = getenv("PROGRAM_PATH");
    std::set<int> Nodes;
    Node task(-1);
    Nodes.insert(-1);
    std::string ans = task.Create(1, programPath);
    Nodes.insert(1);

    // Отправка команды exec 1|abracadabra|mmm
    std::string execCommand = "exec 1|abracadabra|mmm";
    ans = task.Send(execCommand, 1);
    EXPECT_EQ(ans, "Ok:1:-1");
    task.Kill();
}

TEST(Tests, FullTest) {
    std::string programPath = getenv("PROGRAM_PATH");
    std::string ans;
    std::set<int> Nodes;
    Node task(-1);
    Nodes.insert(-1);
    ans = task.Create(1, programPath);
    Nodes.insert(1);
    ans = task.Send("create 2", 1);
    Nodes.insert(2);
    ans = task.Send("create 3", 2);
    Nodes.insert(3);
    
    ans = task.Send("ping 1", 1);
    EXPECT_EQ(ans, "Ok:1");
    ans = task.Send("ping 2", 2);
    EXPECT_EQ(ans, "Ok:1");

    // Тестирование exec на узле 3 с несуществующим шаблоном
    std::string execCommand1 = "exec 3|qweqweqwe|mmm";
    ans = task.Send(execCommand1, 3);
    EXPECT_EQ(ans, "Ok:3:-1");

    // Тестирование exec на узле 3 с существующим шаблоном
    std::string execCommand2 = "exec 3|qweqweqwe|qwe";
    ans = task.Send(execCommand2, 3);
    EXPECT_EQ(ans, "Ok:3:0;3;6");
    task.Kill();
}

int main(int argc, char *argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
