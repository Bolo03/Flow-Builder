#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <windows.h>

#include "./FlowBuilder.h"
#include "./FlowRunner.h"

int main()
{
    // set console title
    SetConsoleTitleA("Flow Builder");

    std::string option;
    // start screen
    while (true)
    {
        system("cls");
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 3);
        std::cout << "Welcome to Flow Builder!" << std::endl;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
        std::cout << "Please choose an option:" << std::endl;
        std::cout << "1. Create a new flow" << std::endl;
        std::cout << "2. Load an existing flow" << std::endl;
        std::cout << "3. Delete an existing flow" << std::endl;
        std::cout << "4. Display analytics for existing flow" << std::endl;
        std::cout << "5. Exit" << std::endl;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 1);
        std::cout << "Option: ";
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
        std::cin >> option;
        fflush(stdin);

        if (option == "1")
        {
            // create a new flow
            FlowBuilder flowBuilder;
            flowBuilder.createFlow();
        }
        else if (option == "2")
        {
            std::string flowName;

            system("cls");
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 1);
            std::cout << "Flow name: ";
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
            getline(std::cin, flowName);
            fflush(stdin);

            // load an existing flow
            FlowRunner flowRunner;
            flowRunner.runFlow(flowName);
                }
        else if (option == "3")
        {
            // delete an existing flow
            std::string flowName;

            system("cls");
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 1);
            std::cout << "Flow name: ";
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
            getline(std::cin, flowName);
            fflush(stdin);

            // check if flow directory exists
            std::string flowDirectory = "./flows/" + flowName;
            if (std::filesystem::exists(flowDirectory))
            {
                // delete flow directory
                std::filesystem::remove_all(flowDirectory);
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 2);
                std::cout << "Flow deleted successfully!" << std::endl;
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
                Sleep(3000);
            }
            else
            {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
                std::cout << "Flow does not exist!" << std::endl;
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
                Sleep(3000);
            }
        }
        else if (option == "4")
        {
            std::string flowName;

            system("cls");
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 1);
            std::cout << "Flow name: ";
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
            getline(std::cin, flowName);
            fflush(stdin);

            // check if flow directory exists
            std::string flowDirectory = "./flows/" + flowName;
            if (std::filesystem::exists(flowDirectory))
            {
                // display analytics
                std::string analyticsFile = flowDirectory + "/analytics.txt";
                std::ifstream file(analyticsFile);
                std::string line;
                while (getline(file, line))
                {
                    std::cout << line << std::endl;
                }
                file.close();

                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8);
                system("pause");
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
            }
            else
            {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
                std::cout << "Flow does not exist!" << std::endl;
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
                Sleep(3000);
            }
        }
        else if (option == "5")
        {
            // exit
            break;
        }
        else
        {
            // invalid option
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
            std::cout << "Invalid option!" << std::endl;
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
            Sleep(3000);
        }
    }

    return 0;
}