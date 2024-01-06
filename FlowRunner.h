#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <regex>
#include <algorithm>
#include <windows.h>

template <typename T>
struct Input
{
    T value;
    int step;
};

struct StepAnalytics
{
    int skips;
    int errors;
};

// min, max
template <typename T>
T calcMin(T a, T b)
{
    return (a < b) ? a : b;
}

template <typename T>
T calcMax(T a, T b)
{
    return (a > b) ? a : b;
}

// abstract class used to write analytics to file
// inherited by FlowRunner
class Flow
{
protected:
    // analytics variables
    int flow_started;
    int flow_finished;
    std::vector<struct StepAnalytics> step_analytics;
    float avg_errors;
    float avg_skips;

    std::string flowName;
    int flowOpened;

    void writeAnalytics()
    {
        std::ifstream analyticsFile;
        std::ofstream newAnalyticsFile;
        analyticsFile.open("./flows/" + flowName + "/analytics.txt");
        newAnalyticsFile.open("./flows/" + flowName + "/new_analytics.txt");

        std::string line;
        // keep title
        std::getline(analyticsFile, line);
        newAnalyticsFile << line << std::endl;
        std::getline(analyticsFile, line); // empty space
        newAnalyticsFile << line << std::endl;

        // update flow started and finished, get the current number from file and add it to the current number
        int current;
        std::getline(analyticsFile, line);
        current = std::stoi(line.substr(38, line.length() - 38)); // 38 is the length of "Number of times the flow has been started: "
        newAnalyticsFile << "Number of times the flow was started: " << current + flow_started << std::endl;

        std::getline(analyticsFile, line);
        current = std::stoi(line.substr(39, line.length() - 39)); // 39 is the length of "Number of times the flow has been finished: "
        newAnalyticsFile << "Number of times the flow was finished: " << current + flow_finished << std::endl;

        flow_finished += current;          // update flow finished for averages
        std::getline(analyticsFile, line); // empty space
        newAnalyticsFile << line << std::endl;

        int skips = 0, errors = 0;
        for (int i = 0; i < step_analytics.size(); i++)
        {
            std::getline(analyticsFile, line);
            newAnalyticsFile << line << std::endl; // step number and type
            std::getline(analyticsFile, line);

            current = std::stoi(line.substr(11, line.length() - 11)); // 11 is the length of "> Skips: "
            newAnalyticsFile << "> Skipped: " << current + step_analytics[i].skips << std::endl;
            // update skips for averages
            skips += current + step_analytics[i].skips;

            std::getline(analyticsFile, line);
            current = std::stoi(line.substr(10, line.length() - 10)); // 10 is the length of "> Errors: "
            newAnalyticsFile << "> Errors: " << current + step_analytics[i].errors << std::endl;

            // update errors for averages
            errors += (current + step_analytics[i].errors);
        }

        std::getline(analyticsFile, line); // empty space
        newAnalyticsFile << std::endl;

        // calculate avg errors and skips per flow completed
        newAnalyticsFile << "Average number of errors per flow completed: " << (float)errors / flow_finished << std::endl;
        newAnalyticsFile << "Average number of skips per flow completed: " << (float)skips / flow_finished << std::endl;

        analyticsFile.close();
        newAnalyticsFile.close();

        // delete old analytics file and rename new analytics file
        std::remove(("./flows/" + flowName + "/analytics.txt").c_str());
        std::rename(("./flows/" + flowName + "/new_analytics.txt").c_str(), ("./flows/" + flowName + "/analytics.txt").c_str());
    }

public:
    Flow()
    {
        flow_started = 0;
        flow_finished = 0;
        step_analytics = std::vector<struct StepAnalytics>();
        avg_errors = 0;
        avg_skips = 0;

        flowName = "";
        flowOpened = 0;
    }

    virtual void runFlow(std::string filename) = 0;

    ~Flow()
    {
        if (flowOpened)
        {
            writeAnalytics();
        }
        step_analytics.clear();
    }

    friend class FlowBuilder;
};

class FlowRunner : public Flow
{
private:
    std::vector<Input<std::string>> textInputs;
    std::vector<Input<float>> numberInputs;
    std::vector<Input<std::string>> calculusResults;
    std::vector<Input<std::string>> textFileInputs;
    std::vector<Input<std::string>> csvFileInputs;

    // 1 to continue, 0 to skip
    int continueOrSkipStep()
    {
        std::string option;
        int valid_option = 0;
        while (!valid_option)
        {

            std::cout << "1. Continue" << std::endl;
            std::cout << "2. Skip" << std::endl;
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 1);
            std::cout << "Option: ";
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
            std::cin >> option;
            fflush(stdin);
            if (option == "1")
            {
                valid_option = 1;
                return 1;
            }
            else if (option == "2")
            {
                valid_option = 1;
                return 0;
            }
            else
            {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
                std::cout << "Invalid option! Please enter a valid option." << std::endl;
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
                Sleep(3000);
            }
        }
        return 0;
    }

    void runTitleStep(std::stringstream &ss, int step_number)
    {
        struct StepAnalytics step_analytic;
        step_analytic.skips = 0;
        step_analytic.errors = 0;

        system("cls");
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8);
        std::cout << "STEP " << step_number << " - "
                  << "TITLE" << std::endl;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);

        std::string title, subtitle;
        getline(ss, title, ',');
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 1);
        std::cout << title << std::endl;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
        std::cout << "----------------------------------------" << std::endl;
        getline(ss, subtitle, ',');
        std::cout << subtitle << std::endl
                  << std::endl;

        // analytics
        step_analytics.push_back(step_analytic);
    }

    void runTextStep(std::stringstream &ss, int step_number)
    {
        struct StepAnalytics step_analytic;
        step_analytic.skips = 0;
        step_analytic.errors = 0;

        system("cls");
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8);
        std::cout << "STEP " << step_number << " - "
                  << "TEXT" << std::endl;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);

        std::string text, copy_text;
        getline(ss, text, ',');
        std::cout << "> " << text << std::endl;
        std::cout << std::endl;
        getline(ss, copy_text, ',');
        std::cout << copy_text << std::endl;
        std::cout << std::endl;

        // analytics
        step_analytics.push_back(step_analytic);
    }

    void runTextInputStep(std::stringstream &ss, int step_number)
    {
        struct StepAnalytics step_analytic;
        step_analytic.skips = 0;
        step_analytic.errors = 0;

        system("cls");
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8);
        std::cout << "STEP " << step_number << " - "
                  << "TEXT_INPUT" << std::endl;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);

        if (!continueOrSkipStep())
        {
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
            std::cout << "Skipped step " << step_number << std::endl;
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
            Sleep(3000);
            return;
        }
        else
        {
            system("cls");
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8);
            std::cout << "STEP " << step_number << " - "
                      << "TEXT_INPUT" << std::endl;
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);

            std::string description;
            getline(ss, description, ',');
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 1);
            std::cout << description << ": " << std::endl;
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
            Input<std::string> input;
            getline(std::cin, input.value);
            fflush(stdin);
            input.step = step_number;
            textInputs.push_back(input);
        }

        // analytics
        step_analytics.push_back(step_analytic);
    }

    void runNumberInputStep(std::stringstream &ss, int step_number)
    {
        struct StepAnalytics step_analytic;
        step_analytic.skips = 0;
        step_analytic.errors = 0;

        system("cls");
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8);
        std::cout << "STEP " << step_number << " - "
                  << "NUMBER_INPUT" << std::endl;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);

        if (!continueOrSkipStep())
        {
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
            std::cout << "Skipped step " << step_number << std::endl;
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);

            step_analytic.skips++; // analytics
            Sleep(3000);

            step_analytics.push_back(step_analytic);
            return;
        }
        else
        {
            system("cls");
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8);
            std::cout << "STEP " << step_number << " - "
                      << "NUMBER_INPUT" << std::endl;
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);

            std::string description;
            getline(ss, description, ',');
            Input<float> input;

            int valid_input = 0;
            while (!valid_input)
            {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 1);
                std::cout << description << ": ";
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
                std::cin >> input.value;
                fflush(stdin);
                if (std::cin.fail())
                {
                    std::cin.clear();
                    std::cin.ignore(1000, '\n');

                    system("cls");
                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
                    std::cout << "Invalid input! Please enter a number." << std::endl;
                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
                    Sleep(1000);

                    // analytics
                    step_analytic.errors++;

                    // Continue or skip step
                    if (!continueOrSkipStep())
                    {
                        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
                        std::cout << "Skipped step " << step_number << std::endl;
                        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);

                        step_analytic.skips++; // analytics
                        Sleep(3000);
                        break;
                    }
                }
                else
                {
                    valid_input = 1;
                    input.step = step_number;
                    numberInputs.push_back(input);
                }
            }
        }

        // analytics
        step_analytics.push_back(step_analytic);
    }

    void runCalculusStep(std::stringstream &ss, int step_number)
    {
        struct StepAnalytics step_analytic;
        step_analytic.skips = 0;
        step_analytic.errors = 0;

        system("cls");
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8);
        std::cout << "STEP " << step_number << " - "
                  << "CALCULUS" << std::endl;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);

        if (!continueOrSkipStep())
        {
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
            std::cout << "Skipped step " << step_number << std::endl;
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);

            step_analytic.skips++; // analytics
            Sleep(3000);

            step_analytics.push_back(step_analytic);
            return;
        }
        else
        {
            std::string description;
            getline(ss, description, ',');

            std::string input;

            std::regex input_regex("step [0-9]+ [\\+\\-\\*\\/] step [0-9]+");
            std::regex min_max_regex("min\\(step [0-9]+, step [0-9]+\\)|max\\(step [0-9]+, step [0-9]+\\)");
            int valid_input = 0;
            while (!valid_input)
            {
                system("cls");
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8);
                std::cout << "STEP " << step_number << " - "
                          << "CALCULUS" << std::endl;
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
                try
                {
                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 1);
                    std::cout << description << ": ";
                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
                    getline(std::cin, input);
                    if (!std::regex_match(input, input_regex) && !std::regex_match(input, min_max_regex))
                    {
                        throw std::invalid_argument("Invalid input! Please enter a valid input.");
                    }
                    else
                    {
                        valid_input = 1;
                        // for +, -, *, /, pattern step 3 + step 5
                        if (std::regex_match(input, input_regex))
                        {
                            // 5 7 14
                            int step1 = std::stoi(input.substr(5, 1));
                            int step2 = std::stoi(input.substr(14, 1));
                            std::string operation = input.substr(7, 1);

                            // check is step1, step2 are number input steps
                            int valid_steps = 0;

                            for (int i = 0; i < numberInputs.size(); i++)
                            {
                                if (numberInputs[i].step == step1)
                                {
                                    step1 = i;
                                    valid_steps++;
                                }
                                if (numberInputs[i].step == step2)
                                {
                                    step2 = i;
                                    valid_steps++;
                                }
                            }

                            if (valid_steps != 2)
                            {
                                throw std::invalid_argument("Invalid input! Please enter existing number steps.");
                            }

                            // calculate
                            float result;
                            std::string result_s;
                            if (operation == "+")
                            {
                                result = numberInputs[step1].value + numberInputs[step2].value;
                                result_s = std::to_string(numberInputs[step1].value) + " + " + std::to_string(numberInputs[step2].value) + " = " + std::to_string(result);
                            }
                            else if (operation == "-")
                            {
                                result = numberInputs[step1].value - numberInputs[step2].value;
                                result_s = std::to_string(numberInputs[step1].value) + " - " + std::to_string(numberInputs[step2].value) + " = " + std::to_string(result);
                            }
                            else if (operation == "*")
                            {
                                result = numberInputs[step1].value * numberInputs[step2].value;
                                result_s = std::to_string(numberInputs[step1].value) + " * " + std::to_string(numberInputs[step2].value) + " = " + std::to_string(result);
                            }
                            else if (operation == "/")
                            {
                                result = numberInputs[step1].value / numberInputs[step2].value;
                                result_s = std::to_string(numberInputs[step1].value) + " / " + std::to_string(numberInputs[step2].value) + " = " + std::to_string(result);
                            }
                            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 2);
                            std::cout << "Result: " << result << std::endl;
                            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 1);

                            Input<std::string> calculusResult;
                            calculusResult.value = result_s;
                            calculusResult.step = step_number;
                            calculusResults.push_back(calculusResult);
                        }
                        // for min, max, pattern min(step 3, step 5)
                        if (std::regex_match(input, min_max_regex))
                        {
                            int step1 = std::stoi(input.substr(9, 1));
                            int step2 = std::stoi(input.substr(17, 1));
                            std::string operation = input.substr(0, 3);

                            // check is step1, step2 are number input steps
                            int valid_steps = 0;

                            for (int i = 0; i < numberInputs.size(); i++)
                            {
                                if (numberInputs[i].step == step1)
                                {
                                    step1 = i;
                                    valid_steps++;
                                }
                                if (numberInputs[i].step == step2)
                                {
                                    step2 = i;
                                    valid_steps++;
                                }
                            }

                            if (valid_steps != 2)
                            {
                                throw std::invalid_argument("Invalid input! Please enter existing number steps.");
                            }

                            // calculate
                            float result;
                            if (operation == "min")
                            {
                                result = calcMin(numberInputs[step1].value, numberInputs[step2].value);
                            }
                            else if (operation == "max")
                            {
                                result = calcMax(numberInputs[step1].value, numberInputs[step2].value);
                            }
                            std::cout << "Result: " << result << std::endl;
                        }
                    }
                }
                catch (std::invalid_argument &e)
                {
                    system("cls");
                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
                    std::cout << e.what() << std::endl;
                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);

                    step_analytic.errors++; // analytics
                    Sleep(1000);
                    // Continue or skip step
                    if (!continueOrSkipStep())
                    {
                        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
                        std::cout << "Skipped step " << step_number << std::endl;
                        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);

                        step_analytic.skips++; // analytics
                        Sleep(3000);
                        break;
                    }
                    else
                    {
                        valid_input = 0;
                    }
                }
            }
            // analytics
            step_analytics.push_back(step_analytic);
        }
    }

    void runDisplayStep(std::stringstream &ss, int step_number)
    {
        struct StepAnalytics step_analytic;
        step_analytic.skips = 0;
        step_analytic.errors = 0;

        system("cls");
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8);
        std::cout << "STEP " << step_number << " - "
                  << "DISPLAY" << std::endl;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);

        if (!continueOrSkipStep())
        {
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
            std::cout << "Skipped step " << step_number << std::endl;
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);

            step_analytic.skips++; // analytics
            Sleep(3000);

            step_analytics.push_back(step_analytic);
            return;
        }
        else
        {
            std::string description;
            getline(ss, description, ',');

            std::string input;
            int valid_input = 0;
            // regex to see if input is .txt or .csv
            std::regex txt_regex(".*\\.txt");
            std::regex csv_regex(".*\\.csv");

            while (!valid_input)
            {
                system("cls");
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8);
                std::cout << "STEP " << step_number << " - "
                          << "DISPLAY" << std::endl;
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);

                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 1);
                std::cout << description << ": ";
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
                getline(std::cin, input);
                try
                {
                    if (!std::regex_match(input, txt_regex) && !std::regex_match(input, csv_regex))
                    {
                        throw std::invalid_argument("Invalid input! Please enter a valid input.");
                    }
                    else
                    {
                        valid_input = 1;
                        std::ifstream file;

                        file.open("./flows/" + flowName + "/" + input);
                        if (!file.is_open())
                        {
                            throw std::invalid_argument("File does not exist!");
                        }
                        std::string line;
                        while (std::getline(file, line))
                        {
                            std::cout << line << std::endl;
                        }
                        file.close();
                    }
                }
                catch (const std::exception &e)
                {
                    system("cls");
                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
                    std::cout << e.what() << std::endl;
                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);

                    step_analytic.errors++; // analytics
                    Sleep(3000);

                    if (!continueOrSkipStep())
                    {
                        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
                        std::cout << "Skipped step " << step_number << std::endl;
                        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);

                        step_analytic.skips++; // analytics
                        Sleep(3000);
                        break;
                    }
                    else
                    {
                        valid_input = 0;
                    }
                }
            }
        }

        // analytics
        step_analytics.push_back(step_analytic);
    }

    void runTextFileInputStep(std::stringstream &ss, int step_number, std::string flowName)
    {
        struct StepAnalytics step_analytic;
        step_analytic.skips = 0;
        step_analytic.errors = 0;

        system("cls");
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8);
        std::cout << "STEP " << step_number << " - "
                  << "TEXT_FILE_INPUT" << std::endl;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);

        if (!continueOrSkipStep())
        {
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
            std::cout << "Skipped step " << step_number << std::endl;
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);

            step_analytic.skips++; // analytics
            Sleep(3000);

            step_analytics.push_back(step_analytic);
            return;
        }
        else
        {
            // copy file from input_files to flows/flow_name
            std::string description;
            getline(ss, description, ',');
            std::string input;
            int valid_input = 0;
            // regex to see if input is .txt
            std::regex txt_regex(".*\\.txt");
            while (!valid_input)
            {
                system("cls");
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8);
                std::cout << "STEP " << step_number << " - "
                          << "TEXT_FILE_INPUT" << std::endl;
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);

                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 1);
                std::cout << description << ": ";
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
                getline(std::cin, input);
                try
                {
                    if (!std::regex_match(input, txt_regex))
                    {
                        throw std::invalid_argument("Invalid input! Please enter a valid input.");
                    }
                    else
                    {
                        valid_input = 1;
                        std::ifstream file;
                        file.open("./input_files/" + input);
                        if (!file.is_open())
                        {
                            throw std::invalid_argument("File does not exist!");
                        }
                        std::string line;
                        std::ofstream new_file;
                        new_file.open("./flows/" + flowName + "/" + input);
                        while (std::getline(file, line))
                        {
                            new_file << line << std::endl;
                        }
                        file.close();
                        new_file.close();

                        // add to vector
                        Input<std::string> textFileInput;
                        textFileInput.value = input;
                        textFileInput.step = step_number;
                        textFileInputs.push_back(textFileInput);

                        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 2);
                        std::cout << "File copied successfully!" << std::endl;
                        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
                    }
                }
                catch (const std::exception &e)
                {
                    system("cls");
                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
                    std::cout << e.what() << std::endl;
                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);

                    step_analytic.errors++; // analytics
                    Sleep(3000);

                    if (!continueOrSkipStep())
                    {
                        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
                        std::cout << "Skipped step " << step_number << std::endl;
                        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);

                        step_analytic.skips++; // analytics
                        Sleep(3000);
                        break;
                    }
                    else
                    {
                        valid_input = 0;
                    }
                }
            }
        }

        // analytics
        step_analytics.push_back(step_analytic);
    }

    void runCSVFileInputStep(std::stringstream &ss, int step_number, std::string flowName)
    {
        struct StepAnalytics step_analytic;
        step_analytic.skips = 0;
        step_analytic.errors = 0;

        system("cls");
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8);
        std::cout << "STEP " << step_number << " - "
                  << "CSV_FILE_INPUT" << std::endl;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);

        if (!continueOrSkipStep())
        {
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
            std::cout << "Skipped step " << step_number << std::endl;
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);

            step_analytic.skips++; // analytics
            Sleep(3000);

            step_analytics.push_back(step_analytic);
            return;
        }
        else
        {
            // copy file from input_files to flows/flow_name
            std::string description;
            getline(ss, description, ',');
            std::string input;
            int valid_input = 0;
            // regex to see if input is .csv
            std::regex txt_regex(".*\\.csv");
            while (!valid_input)
            {
                system("cls");
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8);
                std::cout << "STEP " << step_number << " - "
                          << "CSV_FILE_INPUT" << std::endl;
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);

                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 1);
                std::cout << description << ": ";
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
                getline(std::cin, input);
                try
                {
                    if (!std::regex_match(input, txt_regex))
                    {
                        throw std::invalid_argument("Invalid input! Please enter a valid input.");
                    }
                    else
                    {
                        valid_input = 1;
                        std::ifstream file;
                        file.open("./input_files/" + input);
                        if (!file.is_open())
                        {
                            throw std::invalid_argument("File does not exist!");
                        }
                        std::string line;
                        std::ofstream new_file;
                        new_file.open("./flows/" + flowName + "/" + input);
                        while (std::getline(file, line))
                        {
                            new_file << line << std::endl;
                        }
                        file.close();
                        new_file.close();

                        // add to vector
                        Input<std::string> csvFileInput;
                        csvFileInput.value = input;
                        csvFileInput.step = step_number;
                        csvFileInputs.push_back(csvFileInput);

                        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 2);
                        std::cout << "File copied successfully!" << std::endl;
                        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
                    }
                }
                catch (const std::exception &e)
                {
                    system("cls");
                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
                    std::cout << e.what() << std::endl;
                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);

                    step_analytic.errors++; // analytics
                    Sleep(3000);

                    if (!continueOrSkipStep())
                    {
                        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
                        std::cout << "Skipped step " << step_number << std::endl;
                        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);

                        step_analytic.skips++; // analytics
                        Sleep(3000);
                        break;
                    }
                    else
                    {
                        valid_input = 0;
                    }
                }
            }
        }

        // analytics
        step_analytics.push_back(step_analytic);
    }

    void runOutputStep(std::stringstream &ss, int step_number, std::string flowName)
    {
        struct StepAnalytics step_analytic;
        step_analytic.skips = 0;
        step_analytic.errors = 0;

        system("cls");
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8);
        std::cout << "STEP " << step_number << " - "
                  << "OUTPUT" << std::endl;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);

        if (!continueOrSkipStep())
        {
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
            std::cout << "Skipped step " << step_number << std::endl;
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);

            step_analytic.skips++; // analytics
            Sleep(3000);

            step_analytics.push_back(step_analytic);
            return;
        }
        else
        {
            std::string description;
            getline(ss, description, ',');
            std::string input;
            int valid_input = 0;
            // regex to see if input is .txt
            std::regex txt_regex(".*\\.txt");
            while (!valid_input)
            {
                system("cls");
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8);
                std::cout << "STEP " << step_number << " - "
                          << "OUTPUT" << std::endl;
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);

                // create file in flow folder
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 1);
                std::cout << description << ": ";
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
                getline(std::cin, input);
                fflush(stdin);
                try
                {
                    if (std::regex_match(input, txt_regex))
                    {
                        valid_input = 1;
                        std::ofstream file;
                        // check if file exists
                        std::ifstream checkFile;
                        checkFile.open("./flows/" + flowName + "/" + input);
                        if (checkFile.is_open())
                        {
                            checkFile.close();
                            throw std::invalid_argument("File already exists!");
                        }
                        checkFile.close();

                        file.open("./flows/" + flowName + "/" + input);

                        getline(ss, description, ',');
                        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 1);
                        std::cout << description << ": ";
                        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
                        getline(std::cin, input);
                        fflush(stdin);
                        file << input << std::endl;
                        file << "----------------------------------------" << std::endl;

                        getline(ss, description, ',');
                        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 1);
                        std::cout << description << ": ";
                        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
                        getline(std::cin, input);
                        fflush(stdin);
                        file << input << std::endl
                             << std::endl;

                        int option = 1;
                        while (option)
                        {
                            try
                            {
                                getline(ss, description, ',');
                                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 1);
                                std::cout << description << ": ";
                                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
                                getline(std::cin, input);
                                fflush(stdin);

                                // pattern: step 1
                                std::regex step_regex("step [0-9]");
                                if (std::regex_match(input, step_regex))
                                {
                                    // check if step is text input, number input, calculus, text file input, csv file input
                                    int step = std::stoi(input.substr(5, 1));
                                    int valid_step = 0;
                                    for (int i = 0; i < textInputs.size(); i++)
                                    {
                                        if (textInputs[i].step == step)
                                        {
                                            valid_step = 1;
                                            file << textInputs[i].value << std::endl;
                                        }
                                    }
                                    for (int i = 0; i < numberInputs.size(); i++)
                                    {
                                        if (numberInputs[i].step == step)
                                        {
                                            valid_step = 1;
                                            file << numberInputs[i].value << std::endl;
                                        }
                                    }
                                    for (int i = 0; i < calculusResults.size(); i++)
                                    {
                                        if (calculusResults[i].step == step)
                                        {
                                            valid_step = 1;
                                            file << calculusResults[i].value << std::endl;
                                        }
                                    }
                                    for (int i = 0; i < textFileInputs.size(); i++)
                                    {
                                        if (textFileInputs[i].step == step)
                                        {
                                            valid_step = 1;
                                            std::ifstream textFile;
                                            textFile.open("./flows/" + flowName + "/" + textFileInputs[i].value);
                                            std::string line;
                                            while (std::getline(textFile, line))
                                            {
                                                file << line << std::endl;
                                            }
                                            textFile.close();
                                        }
                                    }
                                    for (int i = 0; i < csvFileInputs.size(); i++)
                                    {
                                        if (csvFileInputs[i].step == step)
                                        {
                                            valid_step = 1;
                                            std::ifstream csvFile;
                                            csvFile.open("./flows/" + flowName + "/" + csvFileInputs[i].value);
                                            std::string line;
                                            while (std::getline(csvFile, line))
                                            {
                                                file << line << std::endl;
                                            }
                                            csvFile.close();
                                        }
                                    }

                                    if (!valid_step)
                                    {
                                        throw std::invalid_argument("Invalid step! Please enter a valid step.");
                                    }
                                }
                                else
                                {
                                    throw std::invalid_argument("Invalid input! Please enter a valid input.");
                                }

                                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 1);
                                std::cout << "Do you want to add another step? (1 - yes, 0 - no): ";
                                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
                                std::cin >> option;
                                fflush(stdin);

                                if (option)
                                {
                                    file << "----------------------------------------" << std::endl;
                                }
                                else
                                {
                                    file.close();
                                    valid_input = 1;
                                    break;
                                }
                            }
                            catch (const std::exception &e)
                            {
                                system("cls");
                                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
                                std::cout << e.what() << std::endl;
                                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);

                                step_analytic.errors++; // analytics
                                Sleep(3000);

                                if (!continueOrSkipStep())
                                {
                                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
                                    std::cout << "Skipped step " << step_number << std::endl;
                                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);

                                    step_analytic.skips++; // analytics
                                    Sleep(3000);
                                    break;
                                }
                                else
                                {
                                    valid_input = 0;
                                }
                            }
                        }
                    }
                    else
                    {
                        throw std::invalid_argument("Invalid input! Please enter a valid input.");
                    }
                }
                catch (std::invalid_argument &e)
                {
                    system("cls");
                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
                    std::cout << e.what() << std::endl;
                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);

                    step_analytic.errors++; // analytics
                    Sleep(3000);

                    if (!continueOrSkipStep())
                    {
                        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
                        std::cout << "Skipped step " << step_number << std::endl;
                        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);

                        step_analytic.skips++; // analytics
                        Sleep(3000);
                        break;
                    }
                    else
                    {
                        valid_input = 0;
                    }
                }
            }
        }

        // analytics
        step_analytics.push_back(step_analytic);
    }

public:
    FlowRunner()
    {
        textInputs = std::vector<Input<std::string>>();
        numberInputs = std::vector<Input<float>>();
        calculusResults = std::vector<Input<std::string>>();
        textFileInputs = std::vector<Input<std::string>>();
        csvFileInputs = std::vector<Input<std::string>>();
    }

    ~FlowRunner()
    {
        textInputs.clear();
        numberInputs.clear();
        calculusResults.clear();
        textFileInputs.clear();
        csvFileInputs.clear();
    }

    void runFlow(std::string flowName)
    {
        Flow::flowName = flowName;

        system("cls");
        // check is flow exists
        std::ifstream flowFile;
        try
        {
            std::string flowPath = "./flows/" + flowName + "/" + flowName + ".csv";
            flowFile.open(flowPath);
            if (!flowFile.is_open())
            {
                throw std::exception();
            }

            std::string line;
            std::vector<std::string> steps;

            flow_started++; // analytics
            flowOpened = 1;

            int step_count = 1;
            while (std::getline(flowFile, line))
            {
                std::stringstream ss(line);
                std::string step;
                getline(ss, step, ',');
                if (step == "TITLE")
                {
                    runTitleStep(ss, step_count);

                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8);
                    system("pause");
                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
                }
                if (step == "TEXT")
                {
                    runTextStep(ss, step_count);

                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8);
                    system("pause");
                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
                }
                if (step == "TEXT_INPUT")
                {
                    runTextInputStep(ss, step_count);

                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8);
                    system("pause");
                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
                }
                if (step == "NUMBER_INPUT")
                {
                    runNumberInputStep(ss, step_count);

                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8);
                    system("pause");
                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
                }
                if (step == "CALCULUS")
                {
                    runCalculusStep(ss, step_count);

                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8);
                    system("pause");
                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
                }
                if (step == "DISPLAY")
                {
                    runDisplayStep(ss, step_count);

                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8);
                    system("pause");
                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
                }
                if (step == "TEXT_FILE_INPUT")
                {
                    runTextFileInputStep(ss, step_count, flowName);

                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8);
                    system("pause");
                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
                }
                if (step == "CSV_FILE_INPUT")
                {
                    runCSVFileInputStep(ss, step_count, flowName);

                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8);
                    system("pause");
                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
                }
                if (step == "OUTPUT")
                {
                    runOutputStep(ss, step_count, flowName);

                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8);
                    system("pause");
                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
                }

                step_count++;
            }

            flowFile.close();
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8);
            system("pause");
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);

            flow_finished++; // analytics
        }
        catch (const std::exception &)
        {
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
            std::cout << "Flow does not exist!" << std::endl;
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
            Sleep(3000);
            return;
        }
    }
};
