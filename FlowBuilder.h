#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <ctime>
#include <filesystem>
#include <windows.h>

class FlowCreator
{
public:
    virtual void createFlow() = 0;
};

class FlowBuilder : public FlowCreator
{
private:
    std::string flowName;
    std::string flowPath;

    std::vector<std::string> steps;
    // all steps methods
    void createTitleStep()
    {
        std::string title, subtitle;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 1);
        std::cout << "Title: ";
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
        getline(std::cin, title);

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 1);
        std::cout << "Subtitle: ";
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
        getline(std::cin, subtitle);

        // write to csv file
        std::ofstream flowFile;
        flowFile.open(flowPath, std::ios_base::app);
        flowFile << "TITLE," << title << "," << subtitle << "\n";
        flowFile.close();

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 2);
        std::cout << "Title step created!" << std::endl;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);

        steps.push_back("TITLE");
        Sleep(3000);
    }

    void createTextStep()
    {
        std::string text, copy_text;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 1);
        std::cout << "Text: ";
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
        getline(std::cin, text);

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 1);
        std::cout << "Copy text: ";
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
        getline(std::cin, copy_text);

        // write to csv file
        std::ofstream flowFile;
        flowFile.open(flowPath, std::ios_base::app);
        flowFile << "TEXT," << text << "," << copy_text << "\n";
        flowFile.close();

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 2);
        std::cout << "Text step created!" << std::endl;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);

        steps.push_back("TEXT");
        Sleep(3000);
    }

    void createTextInputStep()
    {
        std::string description;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 1);
        std::cout << "Description for Text Input: ";
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
        getline(std::cin, description);

        // write to csv file
        std::ofstream flowFile;
        flowFile.open(flowPath, std::ios_base::app);
        flowFile << "TEXT_INPUT," << description << "\n";
        flowFile.close();

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 2);
        std::cout << "Test Input step created!" << std::endl;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);

        steps.push_back("TEXT_INPUT");
        Sleep(3000);
    }

    void createNumberInputStep()
    {
        std::string description;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 1);
        std::cout << "Description for Number Input: ";
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
        getline(std::cin, description);

        // write to csv file
        std::ofstream flowFile;
        flowFile.open(flowPath, std::ios_base::app);
        flowFile << "NUMBER_INPUT," << description << "\n";
        flowFile.close();

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 2);
        std::cout << "Number Input step created!" << std::endl;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);

        steps.push_back("NUMBER_INPUT");
        Sleep(3000);
    }

    void createCalculusStep()
    {
        // check if there are 2 number inputs before
        std::ifstream flowFile(flowPath);
        std::string line;
        int numberInputs = 0;
        while (getline(flowFile, line))
        {
            std::istringstream iss(line);
            std::string word;
            getline(iss, word, ',');
            if (word == "NUMBER_INPUT")
            {
                numberInputs++;
            }
        }
        flowFile.close();

        if (numberInputs < 2)
        {
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
            std::cout << "You need at least 2 number inputs before calculus!" << std::endl;
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);

            Sleep(3000);
            return;
        }
        else
        {
            std::string description;
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 1);
            std::cout << "Description for Calculus input: ";
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
            getline(std::cin, description);

            // write to csv file
            std::ofstream flowFile;
            flowFile.open(flowPath, std::ios_base::app);
            flowFile << "CALCULUS," << description << "\n";
            flowFile.close();

            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 2);
            std::cout << "Calculus step created!" << std::endl;
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
            steps.push_back("CALCULUS");
            Sleep(3000);
        }
    }

    void createDisplayStep()
    {
        // check if there is a file input step before
        std::ifstream flowFile(flowPath);
        std::string line;
        int fileInput = 0;
        while (getline(flowFile, line))
        {
            std::istringstream iss(line);
            std::string word;
            getline(iss, word, ',');
            if (word == "TEXT_FILE_INPUT" || word == "CSV_FILE_INPUT")
            {
                fileInput = 1;
            }
        }
        flowFile.close();

        if (fileInput == 0)
        {
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
            std::cout << "You need at least 1 file input before display!" << std::endl;
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
            Sleep(3000);
            return;
        }
        else
        {
            std::string description;
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 1);
            std::cout << "Description for Display: ";
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
            getline(std::cin, description);

            // write to csv file
            std::ofstream flowFile;
            flowFile.open(flowPath, std::ios_base::app);
            flowFile << "DISPLAY," << description << "\n";
            flowFile.close();

            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 2);
            std::cout << "Display step created!" << std::endl;
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
            steps.push_back("DISPLAY");
            Sleep(3000);
        }
    }

    void createTextFileInputStep()
    {
        std::string description;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 1);
        std::cout << "Description for Text File Input: ";
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
        getline(std::cin, description);

        // write to csv file
        std::ofstream flowFile;
        flowFile.open(flowPath, std::ios_base::app);
        flowFile << "TEXT_FILE_INPUT," << description << "\n";
        flowFile.close();

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 2);
        std::cout << "Text File Input step created!" << std::endl;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);

        steps.push_back("TEXT_FILE_INPUT");
        Sleep(3000);
    }

    void createCSVFileInputStep()
    {
        std::string description;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 1);
        std::cout << "Description for CSV File Input: ";
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
        getline(std::cin, description);

        // write to csv file
        std::ofstream flowFile;
        flowFile.open(flowPath, std::ios_base::app);
        flowFile << "CSV_FILE_INPUT," << description << "\n";
        flowFile.close();

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 2);
        std::cout << "CSV File Input step created!" << std::endl;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);

        steps.push_back("CSV_FILE_INPUT");
        Sleep(3000);
    }

    void createOutputStep()
    {
        std::string name, title, description, information;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 1);
        std::cout << "Description for file name: ";
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
        getline(std::cin, name);

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 1);
        std::cout << "Description for file title: ";
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
        getline(std::cin, title);

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 1);
        std::cout << "Description for file description: ";
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
        getline(std::cin, description);

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 1);
        std::cout << "Description for file information: ";
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
        getline(std::cin, information);

        // write to csv file
        std::ofstream flowFile;
        flowFile.open(flowPath, std::ios_base::app);
        flowFile << "OUTPUT," << name << "," << title << "," << description << "," << information << "\n";
        flowFile.close();

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 2);
        std::cout << "Output step created!" << std::endl;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);

        steps.push_back("OUTPUT");
        Sleep(3000);
    }

    void createEndStep()
    {
        // write to csv file
        std::ofstream flowFile;
        flowFile.open(flowPath, std::ios_base::app);
        flowFile << "END\n";
        flowFile.close();

        // get current time
        time_t now = time(0);
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 2);
        std::cout << "Flow " << flowName << " created at " << ctime(&now) << std::endl;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);

        steps.push_back("END");
        Sleep(3000);
    }

    void createFlowFile()
    {
        int input_valid = 0;
        while (!input_valid)
        {
            system("cls");

            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 1);
            std::cout << "Flow name: ";
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
            std::cin >> flowName;
            fflush(stdin);

            // check if flow already exists
            try
            {
                // create directory and put flow file in it
                std::string directory_name = "./flows/" + flowName;
                if (!std::filesystem::exists(directory_name))
                {
                    input_valid = 1;
                    std::filesystem::create_directory(directory_name);
                    flowPath = directory_name + "/" + flowName + ".csv";
                    std::ofstream flowFile;
                    flowFile.open(flowPath);
                    flowFile.close();
                }
                else
                {
                    throw std::runtime_error("Flow already exists!");
                }
            }
            catch (const std::exception &e)
            {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
                std::cerr << e.what() << '\n';
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
                Sleep(3000);
            }
        }
    }

public:
    FlowBuilder()
    {
        flowName = "";
        flowPath = "";
        steps = {};
    }

    // When flow build is done, create analytics file
    ~FlowBuilder()
    {
        time_t now = time(0);
        std::ofstream analyticsFile;
        analyticsFile.open("./flows/" + flowName + "/" + "analytics.txt");
        // initialize analytics file
        analyticsFile << "Flow " << flowName << " created at " << ctime(&now);
        analyticsFile << "----------------------------------------" << std::endl;
        analyticsFile << "Number of times the flow was started: 0\n";
        analyticsFile << "Number of times the flow was finished: 0\n";
        analyticsFile << "\n";
        for (int i = 0; i < steps.size(); i++)
        {
            analyticsFile << "Step " << i + 1 << ": " << steps[i] << "\n";
            analyticsFile << "> Skipped: 0\n";
            analyticsFile << "> Errors: 0\n";
        }
        analyticsFile << "\n";
        analyticsFile << "Average number of errors per flow finished: 0\n";
        analyticsFile << "Average number of skips per flow finished: 0\n";
        analyticsFile.close();

        steps.clear();
    }

    void createFlow()
    {
        createFlowFile();

        int option;
        while (option != 10) // endstep
        {
            system("cls");
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 1);
            std::cout << "Create your flow. Choose one step: " << std::endl;
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
            std::cout << "1. Title" << std::endl;
            std::cout << "2. Text" << std::endl;
            std::cout << "3. Text Input" << std::endl;
            std::cout << "4. Number Input" << std::endl;
            std::cout << "5. Calculus" << std::endl;
            std::cout << "6. Display" << std::endl;
            std::cout << "7. Text File Input" << std::endl;
            std::cout << "8. CSV File Input" << std::endl;
            std::cout << "9. Output" << std::endl;
            std::cout << "10. End" << std::endl;
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 3);
            std::cout << "Option: ";
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
            std::cin >> option;
            fflush(stdin);

            switch (option)
            {
            case 1:
            {
                system("cls");
                createTitleStep();

                break;
            }
            case 2:
            {
                system("cls");
                createTextStep();

                break;
            }
            case 3:
            {
                system("cls");
                createTextInputStep();

                break;
            }
            case 4:
            {
                system("cls");
                createNumberInputStep();

                break;
            }
            case 5:
            {
                system("cls");
                createCalculusStep();

                break;
            }
            case 6:
            {
                system("cls");
                createDisplayStep();

                break;
            }
            case 7:
            {
                system("cls");
                createTextFileInputStep();

                break;
            }
            case 8:
            {
                system("cls");
                createCSVFileInputStep();

                break;
            }
            case 9:
            {
                system("cls");
                createOutputStep();

                break;
            }
            case 10:
            {
                system("cls");
                createEndStep();

                break;
            }
            default:
            {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
                std::cout << "Invalid option!" << std::endl;
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
                Sleep(3000);
                break;
            }
            }
        }
    }
};