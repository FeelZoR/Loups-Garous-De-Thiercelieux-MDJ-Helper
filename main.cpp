#include <SFGUI/SFGUI.hpp>
#include <SFGUI/Widgets.hpp>

#include <SFML/Graphics.hpp>

#include <fstream>
#include <iostream>
#include <algorithm>
#include <string>
#include <random>
#include <chrono>
#include <windows.h>
#include <cstdlib>
#include <ctime>

struct Data {
    std::vector<std::string> colors;
    std::vector<std::string> flasks;
    std::vector<std::string> events;
    std::vector<std::string> questions;
    std::vector<std::string> answers;

    std::map<std::string,std::string> associatedQuestions;
    std::map<std::string,std::string> coloredFlasks;
    std::deque<int> questionOrder;
};

void createNewLabel(auto text, auto box, sf::Vector2f alignment = sf::Vector2f(0.5f, 0.5f));
std::string toLowerCase(const std::string& target);

// Creates the box that contains the whole creation part (left part of the window).
void setCreationBox(auto creationBox, Data& data, auto colorList, auto effectList, auto eventList, auto questionList, auto answerList);

// Creates the part to define the colours and the effects.
void setFlaskBox(auto creationBox, Data& data, auto colorList, auto effectList);

// Creates the box that contains the color creation.
void setColorCreation(auto colorBox, Data& data, auto colorList);

// For both flask and color creation parts.
void setLoadedWindow(auto colorBox, auto colorList, sf::Vector2f requisition = sf::Vector2f(115.f, 150.f));

// Loads the file that contains all the colors.
int loadColorFile(std::vector<std::string> &colors, auto colorsLoadedBox);

// Creates the entry necessary to add new colors.
void setColorAdd(auto colorBox, Data& data, auto colorList);

// Adds a color to the color list if it doesn't exist and returns true. Returns false if the color already exists.
bool addColor(std::string name, Data& data);

// Creates the box that contains the flask creation.
void setFlaskCreation(auto flaskBox, Data& data, auto effectList);

// Loads the file that contains all the flask effects.
int loadFlaskFile(std::vector<std::string>& effects, auto effectList);

// Creates the entry necessary to add new effects.
void setFlaskAdd(auto flaskBox, Data& data, auto effectList);

// Adds a flask to the flask list if it doesn't exist and returns true. Returns false if the flask already exists.
bool addFlask(std::string name, Data& data);

// Creates the part to define the different events.
void setEventBox(auto creationBox, Data& data, auto eventList);

// Creates the box that contains the event creation.
void setEventCreation(auto eventBox, Data& data, auto eventList);

// Loads the file that contains all the events.
int loadEventFile(std::vector<std::string>& events, auto eventList);

// Creates the entry necessary to add new events.
void setEventAdd(auto eventBox, Data& data, auto eventList);

// Adds an event to the event list if it doesn't exist and returns true. Returns false if the event already exists.
bool addEvent(std::string name, Data& data);

// Creates the part to define the different questions.
void setQuestionBox(auto creationBox, Data& data, auto questionList, auto answerList);

// Create the two lists, one for the questions and one for the answers.
void setQuestionLists(auto listBox, Data& data, auto questionList, auto answerList);

// Loads the file that contains all the questions.
int loadQuestionFile(std::vector<std::string> &questions, auto questionBox);

// Loads the file that contains all the answers.
int loadAnswerFile(std::vector<std::string> &answers, auto answerBox);

// Creates the entries necessary to add questions.
void createEntries(auto entryBox, Data& data, auto questionList, auto answerList);

// Adds a question and its answer to their respective list if the question doesn't exist and returns true. Returns false if the question already exists.
bool addQuestion(std::string question, std::string answer, Data& data);

// Creates the box that contains the whole turn part (the right of the window).
void setTurnBox(auto turnBox, Data& data, auto questionLabel, auto answerLabel, auto colorChoice, auto rewardLabel, auto eventLabel);

// Creates the panel where we can start a new game and a new turn.
void createNewPanel(auto turnBox, Data& data, auto questionLabel, auto answerLabel, auto colorChoice, auto rewardLabel, auto eventLabel);

// Creates a new game.
void newGame(Data& data, auto questionLabel, auto answerLabel, auto colorChoice, auto rewardLabel, auto eventlabel);

// Associate all the questions with their respective answers.
void associateQuestions(Data& data);

// Defines the question order.
void mixQuestions(Data& data);

// Mix all the rewards given by the flasks.
void mixFlasks(Data& data);

// Associates a reward with a colour.
void associateFlasks(Data& data);

// Sets all the turn labels to nothing.
void resetLabels(auto questionLabel, auto answerLabel, auto rewardLabel, auto eventLabel);

// Sets all the colours back to normal
void resetColors(Data& data, auto colorChoice);

// Starts a new turn.
void newTurn(Data& data, auto questionLabel, auto answerLabel, auto rewardLabel, auto eventLabel);

// Chooses the next question to show.
void chooseNextQuestion(Data& data, auto questionLabel, auto answerLabel);

// Creates the box that will show the question and its answer.
void createQuestionBox(auto turnBox, auto questionLabel, auto answerLabel, Data& data);

// Creates the box that will show the colors and let the user select one.
void createColorBox(auto turnBox, auto colorChoice, Data& data, auto rewardLabel);

// Copies all the available colors in the clipboard.
void copyColors(auto colorChoice);

// Copies the specified text into the clipboard.
void toClipboard(const std::string &s);

// Uses the selected color.
void useColor(Data& data, auto colorChoice, auto rewardLabel);

// Creates the box that will show the rewards.
void createRewardBox(auto turnBox, auto rewardLabel);

// Creates the box that will show the events.
void createEventBox(auto turnBox, auto eventLabel);

// Randomly selects an event.
void chooseRandomEvent(Data& data, auto eventLabel);

int main() {
    srand(time(NULL));
    Data data;

	sf::RenderWindow render_window(sf::VideoMode(1024, 676), "Werewolves of Thiercellieux GM helper.", sf::Style::Close);
	render_window.resetGLStates();

	sfg::SFGUI sfgui;
	sfg::Desktop desktop;

    auto window = sfg::Window::Create(sfg::Window::Style::BACKGROUND);
    desktop.Add(window);

    // This is the box that will contain the whole program, that is organized into two vertical boxes, one against the other.
    auto globalBox = sfg::Box::Create(sfg::Box::Orientation::HORIZONTAL, 3.0f);
    globalBox->SetRequisition(sf::Vector2f(1002.f, 654.f)); // Strangely, it fills completely the window that way.
    window->Add(globalBox);

    // This is the creation box, where the GM will create all the elements of the game.
    auto creationBox = sfg::Box::Create(sfg::Box::Orientation::VERTICAL, 5.0f);

    auto colorList = sfg::Box::Create(sfg::Box::Orientation::VERTICAL); // Initialized here so it can be updated later.
    auto effectList = sfg::Box::Create(sfg::Box::Orientation::VERTICAL); // Initialized here so it can be updated later.
    auto eventList = sfg::Box::Create(sfg::Box::Orientation::VERTICAL); // Initialized here so it can be updated later.
    auto questionList = sfg::Box::Create(sfg::Box::Orientation::VERTICAL); // Initialized here so it can be updated later.
    auto answerList = sfg::Box::Create(sfg::Box::Orientation::VERTICAL); // Initialized here so it can be updated later.

    setCreationBox(creationBox, data, colorList, effectList, eventList, questionList, answerList);
    globalBox->Pack(creationBox);

    auto verticalSeparator = sfg::Separator::Create(sfg::Separator::Orientation::VERTICAL);
    globalBox->Pack(verticalSeparator);

    // This is the turn box, where all the turns will be handled.
    auto turnBox = sfg::Box::Create(sfg::Box::Orientation::VERTICAL, 5.0f);

    auto questionLabel = sfg::Label::Create(""); // Initialized here so it can be updated later.
    auto answerLabel = sfg::Label::Create(""); // Initialized here so it can be updated later.
    auto colorChoice = sfg::ComboBox::Create(); // Initialized here so it can be updated later.
    auto rewardLabel = sfg::Label::Create(""); // Initialized here so it can be updated later.
    auto eventLabel = sfg::Label::Create(""); // Initialized here so it can be updated later.

    setTurnBox(turnBox, data, questionLabel, answerLabel, colorChoice, rewardLabel, eventLabel);
    globalBox->Pack(turnBox);

    newGame(data, questionLabel, answerLabel, colorChoice, rewardLabel, eventLabel);

    sf::Event event;
    sf::Clock clock;

    while(render_window.isOpen()) {
        while(render_window.pollEvent(event)) {
            desktop.HandleEvent(event);

            if(event.type == sf::Event::Closed) {
                return 0;
            }
        }

        desktop.Update(clock.restart().asSeconds());

        render_window.clear();
        sfgui.Display(render_window);
        render_window.display();
    }

	return 0;
}

void createNewLabel(auto text, auto box, sf::Vector2f alignment) {
    auto label = sfg::Label::Create(text);
    label->SetAlignment(alignment);
    box->Pack(label);
}

std::string toLowerCase(const std::string& target) {
    std::string result;
    for (unsigned int i = 0; i < target.size(); i++) {
        result += std::tolower(target[i]);
    }

    return result;
}

void setCreationBox(auto creationBox, Data& data, auto colorList, auto effectList, auto eventList, auto questionList, auto answerList) {
    setFlaskBox(creationBox, data, colorList, effectList);
    setEventBox(creationBox, data, eventList);
    setQuestionBox(creationBox, data, questionList, answerList);
}

void setFlaskBox(auto creationBox, Data& data, auto colorList, auto effectList) {
    auto flaskBox = sfg::Box::Create(sfg::Box::Orientation::HORIZONTAL, 10.0f);
    auto colorBox = sfg::Box::Create(sfg::Box::Orientation::VERTICAL, 5.0f);
    auto flaskEffectBox = sfg::Box::Create(sfg::Box::Orientation::VERTICAL, 5.0f);

    setColorCreation(colorBox, data, colorList);
    setFlaskCreation(flaskEffectBox, data, effectList);

    flaskBox->Pack(colorBox);
    flaskBox->Pack(flaskEffectBox);

	auto flaskFrame = sfg::Frame::Create("Flask creation");
    flaskFrame->SetAlignment(sf::Vector2f(0.2f, 0.5f));
    flaskFrame->Add(flaskBox);
    creationBox->Pack(flaskFrame, false, false);
}

void setColorCreation(auto colorBox, Data& data, auto colorList) {
    loadColorFile(data.colors, colorList);
    setLoadedWindow(colorBox, colorList);
    setColorAdd(colorBox, data, colorList);
}

void setLoadedWindow(auto box, auto loadedBox, sf::Vector2f requisition) {
    loadedBox->SetRequisition(sf::Vector2f(91.f, 0.f));

    auto loadedWindow = sfg::ScrolledWindow::Create();
    loadedWindow->SetScrollbarPolicy(sfg::ScrolledWindow::HORIZONTAL_AUTOMATIC | sfg::ScrolledWindow::VERTICAL_ALWAYS);
	loadedWindow->SetRequisition(requisition);
	loadedWindow->AddWithViewport(loadedBox);

	box->Pack(loadedWindow, false, true);
}

int loadColorFile(std::vector<std::string> &colors, auto colorsLoadedBox) {
    std::ifstream colorsFile("res/colors.txt");
    if (colorsFile) {
        std::string tempText;
        while (std::getline(colorsFile, tempText)) {
            if (!tempText.empty()) {
                colors.push_back(tempText);
                createNewLabel(tempText, colorsLoadedBox);
            }
        }

        return 0;
    }

    // An error occurred
    return 1;
}

void setColorAdd(auto colorBox, Data& data, auto colorList) {
    auto entryBox = sfg::Box::Create(sfg::Box::Orientation::HORIZONTAL, 5.0f);
    auto colorEntry = sfg::Entry::Create();
    colorEntry->SetRequisition(sf::Vector2f(75.f, 0.f));
    auto colorSubmit = sfg::Button::Create("Add");

    colorSubmit->GetSignal( sfg::Widget::OnLeftClick ).Connect([colorEntry, colorList, &data] {
                                                                std::string text = colorEntry->GetText().toAnsiString();
                                                                if (addColor(text, data)) {
                                                                    createNewLabel(text, colorList);
                                                                }
                                                                colorEntry->SetText("");
                                                            });

    entryBox->Pack(colorEntry);
    entryBox->Pack(colorSubmit);
    colorBox->Pack(entryBox, true, false);
}

bool addColor(std::string name, Data& data) {
    if (std::find(data.colors.begin(), data.colors.end(), name) == data.colors.end()) {
        std::ofstream colorsFile("res/colors.txt");
        data.colors.push_back(name);
        if (colorsFile) {
            for (std::string color : data.colors) {
                colorsFile << color << std::endl;
            }
        }

        return true;
    }

    return false;
}

void setFlaskCreation(auto flaskBox, Data& data, auto effectList) {
    loadFlaskFile(data.flasks, effectList);
    setLoadedWindow(flaskBox, effectList, sf::Vector2f(340.f, 150.f));
    setFlaskAdd(flaskBox, data, effectList);
}

int loadFlaskFile(std::vector<std::string>& effects, auto effectList) {
    std::ifstream effectsFile("res/rewards.txt");
    if (effectsFile) {
        std::string tempText;
        while (std::getline(effectsFile, tempText)) {
            if (!tempText.empty()) {
                effects.push_back(tempText);
                createNewLabel(tempText, effectList, sf::Vector2f(0.f, 0.f));
            }
        }
        return 0;
    }

    // An error occurrs
    return 1;
}

void setFlaskAdd(auto flaskBox, Data& data, auto effectList) {
    auto entryBox = sfg::Box::Create(sfg::Box::Orientation::HORIZONTAL, 10.0f);
    auto flaskEntry = sfg::Entry::Create();
    flaskEntry->SetRequisition(sf::Vector2f(290.f, 0.f));
    auto flaskSubmit = sfg::Button::Create("Add");

    flaskSubmit->GetSignal( sfg::Widget::OnLeftClick ).Connect([flaskEntry, effectList, &data] {
                                                                std::string text = flaskEntry->GetText().toAnsiString();
                                                                if (addFlask(text, data)) {
                                                                    createNewLabel(text, effectList, sf::Vector2f(0.f, 0.f));
                                                                }
                                                                flaskEntry->SetText("");
                                                            });

    entryBox->Pack(flaskEntry);
    entryBox->Pack(flaskSubmit);
    flaskBox->Pack(entryBox, true, false);
}

bool addFlask(std::string name, Data& data) {
    if (std::find(data.flasks.begin(), data.flasks.end(), name) == data.flasks.end()) {
        std::ofstream flasksFile("res/rewards.txt");
        data.flasks.push_back(name);
        if (flasksFile) {
            for (std::string flask : data.flasks) {
                flasksFile << flask << std::endl;
            }
        }
        return true;
    }
    return false;
}

void setEventBox(auto creationBox, Data& data, auto eventList) {
    auto eventBox = sfg::Box::Create(sfg::Box::Orientation::VERTICAL, 5.0f);
    setEventCreation(eventBox, data, eventList);

    auto eventFrame = sfg::Frame::Create("Event creation");
    eventFrame->SetAlignment(sf::Vector2f(0.2f, 0.5f));
    eventFrame->Add(eventBox);
    creationBox->Pack(eventFrame, false, false);
}

void setEventCreation(auto eventBox, Data& data, auto eventList) {
    loadEventFile(data.events, eventList);
    setLoadedWindow(eventBox, eventList, sf::Vector2f(400.f, 150.f));
    setEventAdd(eventBox, data, eventList);
}

int loadEventFile(std::vector<std::string>& events, auto eventList) {
    std::ifstream eventFile("res/events.txt");
    if (eventFile) {
        std::string tempText;
        while (std::getline(eventFile, tempText)) {
            if (!tempText.empty()) {
                events.push_back(tempText);
                createNewLabel(tempText, eventList, sf::Vector2f(0.f, 0.f));
            }
        }

        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        std::shuffle(std::begin(events), std::end(events), std::default_random_engine(seed));
        return 0;
    }

    // An error occurrs
    return 1;
}

void setEventAdd(auto eventBox, Data& data, auto eventList) {
    auto entryBox = sfg::Box::Create(sfg::Box::Orientation::HORIZONTAL, 10.0f);
    auto eventEntry = sfg::Entry::Create();
    eventEntry->SetRequisition(sf::Vector2f(290.f, 0.f));
    auto eventSubmit = sfg::Button::Create("Add");

    eventSubmit->GetSignal( sfg::Widget::OnLeftClick ).Connect([eventEntry, eventList, &data] {
                                                                std::string text = eventEntry->GetText().toAnsiString();
                                                                if (addEvent(text, data)) {
                                                                    createNewLabel(text, eventList, sf::Vector2f(0.f, 0.f));
                                                                }
                                                                eventEntry->SetText("");
                                                            });

    entryBox->Pack(eventEntry);
    entryBox->Pack(eventSubmit);
    eventBox->Pack(entryBox, true, false);
}

bool addEvent(std::string name, Data& data) {
    if (std::find(data.events.begin(), data.events.end(), name) == data.events.end()) {
        std::ofstream eventsFile("res/events.txt");
        data.events.push_back(name);
        if (eventsFile) {
            for (std::string event : data.events) {
                eventsFile << event << std::endl;
            }
        }
        return true;
    }
    return false;
}

void setQuestionBox(auto creationBox, Data& data, auto questionList, auto answerList) {
    auto questionBox = sfg::Box::Create(sfg::Box::Orientation::VERTICAL, 5.0f);
    auto listBox = sfg::Box::Create(sfg::Box::Orientation::HORIZONTAL, 10.0f);
    auto entryBox = sfg::Box::Create(sfg::Box::Orientation::HORIZONTAL, 10.0f);

    setQuestionLists(listBox, data, questionList, answerList);
    createEntries(entryBox, data, questionList, answerList);

    questionBox->Pack(listBox);
    questionBox->Pack(entryBox);

	auto questionFrame = sfg::Frame::Create("Question creation");
    questionFrame->SetAlignment(sf::Vector2f(0.2f, 0.5f));
    questionFrame->Add(questionBox);
    creationBox->Pack(questionFrame, false, false);
}

void setQuestionLists(auto listBox, Data& data, auto questionList, auto answerList) {
    loadQuestionFile(data.questions, questionList);
    loadAnswerFile(data.answers, answerList);
    setLoadedWindow(listBox, questionList, sf::Vector2f(340.f, 150.f));
    setLoadedWindow(listBox, answerList);
}

int loadQuestionFile(std::vector<std::string> &questions, auto questionBox) {
    std::ifstream questionFile("res/questions.txt");
    if (questionFile) {
        std::string tempText;
        while (std::getline(questionFile, tempText)) {
            if (!tempText.empty()) {
                questions.push_back(tempText);
                createNewLabel(tempText, questionBox, sf::Vector2f(0.f, 0.f));
            }
        }

        return 0;
    }

    // An error occurred
    return 1;
}

int loadAnswerFile(std::vector<std::string> &answers, auto answerBox) {
    std::ifstream answerFile("res/answers.txt");
    if (answerFile) {
        std::string tempText;
        while (std::getline(answerFile, tempText)) {
            if (!tempText.empty()) {
                answers.push_back(tempText);
                createNewLabel(tempText, answerBox, sf::Vector2f(0.f, 0.f));
            }
        }

        return 0;
    }

    // An error occurred
    return 1;
}

void createEntries(auto entryBox, Data& data, auto questionList, auto answerList) {
    auto questionEntry = sfg::Entry::Create();
    questionEntry->SetRequisition(sf::Vector2f(80.f, 0.f));

    auto answerEntry = sfg::Entry::Create();
    answerEntry->SetRequisition(sf::Vector2f(80.f, 0.f));

    auto submit = sfg::Button::Create("Add");

    submit->GetSignal( sfg::Widget::OnLeftClick ).Connect([questionEntry, questionList, answerEntry, answerList, &data] {
                                                                std::string question = questionEntry->GetText().toAnsiString();
                                                                std::string answer = answerEntry->GetText().toAnsiString();
                                                                if (addQuestion(question, answer, data)) {
                                                                    createNewLabel(question, questionList, sf::Vector2f(0.f, 0.f));
                                                                    createNewLabel(answer, answerList, sf::Vector2f(0.f, 0.f));
                                                                }
                                                                questionEntry->SetText("");
                                                                answerEntry->SetText("");
                                                            });

    entryBox->Pack(questionEntry);
    entryBox->Pack(answerEntry);
    entryBox->Pack(submit);
}

bool addQuestion(std::string question, std::string answer, Data& data) {
    if (std::find(data.questions.begin(), data.questions.end(), question) == data.questions.end() && question.size() != 0 && answer.size() != 0) {
        std::ofstream questionsFile("res/questions.txt");
        data.questions.push_back(question);
        if (questionsFile) {
            for (std::string question : data.questions) {
                questionsFile << question << std::endl;
            }
        }

        std::ofstream answersFile("res/answers.txt");
        data.answers.push_back(answer);
        if (answersFile) {
            for (std::string answer : data.answers) {
                answersFile << answer << std::endl;
            }
        }

        return true;
    }

    return false;
}

void setTurnBox(auto turnBox, Data& data, auto questionLabel, auto answerLabel, auto colorChoice, auto rewardLabel, auto eventLabel) {
    createNewPanel(turnBox, data, questionLabel, answerLabel, colorChoice, rewardLabel, eventLabel);
    createQuestionBox(turnBox, questionLabel, answerLabel, data);
    createColorBox(turnBox, colorChoice, data, rewardLabel);
    createRewardBox(turnBox, rewardLabel);
    createEventBox(turnBox, eventLabel);
}

void createNewPanel(auto turnBox, Data& data, auto questionLabel, auto answerLabel, auto colorChoice, auto rewardLabel, auto eventLabel) {
    auto newPanelBox = sfg::Box::Create(sfg::Box::Orientation::HORIZONTAL, 0.f);

    auto newGameButton = sfg::Button::Create("New Game");
    newPanelBox->Pack(newGameButton);
    auto newTurnButton = sfg::Button::Create("New Turn");
    newPanelBox->Pack(newTurnButton);

    newGameButton->GetSignal( sfg::Widget::OnLeftClick ).Connect([&data, questionLabel, answerLabel, colorChoice, rewardLabel, eventLabel] {
                                                                newGame(data, questionLabel, answerLabel, colorChoice, rewardLabel, eventLabel);
                                                            });

    newTurnButton->GetSignal( sfg::Widget::OnLeftClick ).Connect([&data, questionLabel, answerLabel, colorChoice, rewardLabel, eventLabel] {
                                                                newTurn(data, questionLabel, answerLabel, rewardLabel, eventLabel);
                                                            });

    turnBox->Pack(newPanelBox, false, false);
}

void newGame(Data& data, auto questionLabel, auto answerLabel, auto colorChoice, auto rewardLabel, auto eventLabel) {
    associateQuestions(data);
    mixQuestions(data);
    mixFlasks(data);
    associateFlasks(data);

    resetLabels(questionLabel, answerLabel, rewardLabel, eventLabel);
    resetColors(data, colorChoice);

    newTurn(data, questionLabel, answerLabel, rewardLabel, eventLabel);
}

void mixQuestions(Data& data) {
    data.questionOrder = std::deque<int>(data.associatedQuestions.size());
    std::iota(std::begin(data.questionOrder), std::end(data.questionOrder), 0);

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::shuffle(std::begin(data.questionOrder), std::end(data.questionOrder), std::default_random_engine(seed));
}

void associateQuestions(Data& data) {
    for (int i = 0; i < std::min(data.answers.size(), data.questions.size()); i++) {
        data.associatedQuestions[data.questions[i]] = data.answers[i];
    }
}

void mixFlasks(Data& data) {
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::shuffle(std::begin(data.flasks), std::end(data.flasks), std::default_random_engine(seed));
}

void associateFlasks(Data& data) {
    for (int i = 0; i < data.colors.size(); i++) {
        if (i < data.flasks.size()) data.coloredFlasks[data.colors[i]] = data.flasks[i];
        else data.coloredFlasks[data.colors[i]] = "Vide";
    }
}

void resetLabels(auto questionLabel, auto answerLabel, auto rewardLabel, auto eventLabel) {
    questionLabel->SetText("");
    answerLabel->SetText("");
    rewardLabel->SetText("");
    eventLabel->SetText("");
}

void resetColors(Data& data, auto colorChoice) {
    colorChoice->Clear();
    for(std::string color : data.colors) {
        colorChoice->AppendItem(color);
    }
    colorChoice->SelectItem(0);
}

void newTurn(Data& data, auto questionLabel, auto answerLabel, auto rewardLabel, auto eventLabel) {
    chooseNextQuestion(data, questionLabel, answerLabel);
    chooseRandomEvent(data, eventLabel);
}

void chooseNextQuestion(Data& data, auto questionLabel, auto answerLabel) {
    if (data.associatedQuestions.size() > 0) {
        std::map<std::string, std::string>::iterator it = data.associatedQuestions.find(data.questions[data.questionOrder[0]]);
        data.questionOrder.pop_front();
        questionLabel->SetText(it->first);
        answerLabel->SetText(it->second);
        data.associatedQuestions.erase(it);
    } else {
        questionLabel->SetText("No question available");
        answerLabel->SetText("");
    }
}

void createQuestionBox(auto turnBox, auto questionLabel, auto answerLabel, Data& data) {
    auto globalBox = sfg::Box::Create(sfg::Box::Orientation::VERTICAL, 5.0f);

    auto separatorTop = sfg::Separator::Create(sfg::Separator::Orientation::HORIZONTAL);
    separatorTop->SetRequisition(sf::Vector2f(510.f, 0.f));
    auto separatorMid = sfg::Separator::Create(sfg::Separator::Orientation::HORIZONTAL);
    auto separatorBot = sfg::Separator::Create(sfg::Separator::Orientation::HORIZONTAL);

    questionLabel->SetRequisition(sf::Vector2f(510.f, 100.f));
    questionLabel->SetLineWrap(true);

    questionLabel->GetSignal( sfg::Widget::OnLeftClick ).Connect([questionLabel] {
                                                                toClipboard(questionLabel->GetText().toAnsiString());
                                                            });

    answerLabel->SetRequisition(sf::Vector2f(510.f, 100.f));
    answerLabel->SetLineWrap(true);

    answerLabel->GetSignal( sfg::Widget::OnLeftClick ).Connect([answerLabel] {
                                                                toClipboard(answerLabel->GetText().toAnsiString());
                                                            });

    globalBox->Pack(separatorTop, false, false);
    globalBox->Pack(questionLabel, false, false);
    globalBox->Pack(separatorMid, false, false);
    globalBox->Pack(answerLabel, false, false);
    globalBox->Pack(separatorBot, false, false);

    auto changeQuestionButton = sfg::Button::Create("Change Question");
    changeQuestionButton->GetSignal( sfg::Widget::OnLeftClick ).Connect([questionLabel, answerLabel, &data] {
                                                                chooseNextQuestion(data, questionLabel, answerLabel);
                                                            });

    globalBox->Pack(changeQuestionButton, false, false);

    turnBox->Pack(globalBox, false, false);
}

void createColorBox(auto turnBox, auto colorChoice, Data& data, auto rewardLabel) {
    auto colorFrame = sfg::Frame::Create("Color choice");
    colorFrame->SetAlignment(sf::Vector2f(0.8f, 0.5f));

    colorChoice->SetRequisition(sf::Vector2f(50.f, 0.f));

    auto frameBox = sfg::Box::Create(sfg::Box::Orientation::HORIZONTAL, 5.0f);
    auto buttonBox = sfg::Box::Create(sfg::Box::Orientation::VERTICAL, 10.0f);

    auto copyButton = sfg::Button::Create("Copy");
    auto useButton = sfg::Button::Create("Use");

    buttonBox->Pack(copyButton);
    buttonBox->Pack(useButton);

    frameBox->Pack(colorChoice);
    frameBox->Pack(buttonBox);

    colorFrame->Add(frameBox);
    turnBox->Pack(colorFrame, false, false);

    copyButton->GetSignal( sfg::Widget::OnLeftClick ).Connect([colorChoice] {
                                                                copyColors(colorChoice);
                                                            });

    useButton->GetSignal( sfg::Widget::OnLeftClick ).Connect([&data, colorChoice, rewardLabel] {
                                                                useColor(data, colorChoice, rewardLabel);
                                                            });
}

void copyColors(auto colorChoice) {
    std::string textToCopy = "";
    for (unsigned int i = 0; i < colorChoice->GetItemCount(); i++) {
        textToCopy += colorChoice->GetItem(i).toAnsiString() + " ";
    }

    toClipboard(textToCopy);
}

void toClipboard(const std::string &s) {
	OpenClipboard(0);
	EmptyClipboard();
	HGLOBAL hg=GlobalAlloc(GMEM_MOVEABLE,s.size()+1);
	if (!hg){
		CloseClipboard();
		return;
	}
	memcpy(GlobalLock(hg),s.c_str(),s.size()+1);
	GlobalUnlock(hg);
	SetClipboardData(CF_TEXT,hg);
	CloseClipboard();
	GlobalFree(hg);
}

void useColor(Data& data, auto colorChoice, auto rewardLabel) {
    rewardLabel->SetText(data.coloredFlasks[colorChoice->GetSelectedText().toAnsiString()]);

    colorChoice->RemoveItem(colorChoice->GetSelectedItem());
    colorChoice->SelectItem(0);
}

void createRewardBox(auto turnBox, auto rewardLabel) {
    auto rewardFrame = sfg::Frame::Create("Reward");
    rewardFrame->SetAlignment(sf::Vector2f(0.8f, 0.5f));

    rewardLabel->SetRequisition(sf::Vector2f(500.f, 100.f));
    rewardLabel->SetLineWrap(true);

    rewardLabel->GetSignal( sfg::Widget::OnLeftClick ).Connect([rewardLabel] {
                                                                toClipboard(rewardLabel->GetText().toAnsiString());
                                                            });

    rewardFrame->Add(rewardLabel);
    turnBox->Pack(rewardFrame, false, false);
}

void createEventBox(auto turnBox, auto eventLabel) {
    auto eventFrame = sfg::Frame::Create("Event");
    eventFrame->SetAlignment(sf::Vector2f(0.8f, 0.5f));

    eventLabel->SetRequisition(sf::Vector2f(500.f, 100.f));
    eventLabel->SetLineWrap(true);

    eventLabel->GetSignal( sfg::Widget::OnLeftClick ).Connect([eventLabel] {
                                                                toClipboard(eventLabel->GetText().toAnsiString());
                                                            });

    eventFrame->Add(eventLabel);
    turnBox->Pack(eventFrame, false, false);
}

void chooseRandomEvent(Data& data, auto eventLabel) {
    if (rand() % 7 == 0 && data.events.size() > 0) {
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        std::shuffle(std::begin(data.events), std::end(data.events), std::default_random_engine(seed));

        eventLabel->SetText(data.events[0]);
    } else {
        eventLabel->SetText("No event this turn.");
    }
}
