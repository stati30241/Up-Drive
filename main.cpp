#include <SFML/Graphics.hpp>

#include <fstream>
#include <string>
#include <algorithm>
#include <random>


struct Question {
	std::string question;
	std::vector<std::string> choices;
	size_t correctAnswer;
};


struct AnswerDisplay {
	std::string answer;
	sf::Text text;
	

	AnswerDisplay(const std::string& answer, sf::Font* font, int i) {
		text.setFont(*font);
		text.setPosition(50.0f, i * 100.0f + 150.0f);
		text.setString(answer);
	}

	void render(sf::RenderTarget& target) {
		target.draw(text);
	}
};


struct QuestionDisplay {
	Question question;
	sf::Sprite questionLeftSide;
	sf::Sprite questionRightSide;
	sf::Sprite questionMiddle;
	std::vector<AnswerDisplay> answers;
	sf::Text text;

	QuestionDisplay(const Question& q, sf::Font* font, sf::Texture* texture) {
		question = q;
		text.setFont(*font);
		text.setPosition(50.0f, 50.0f);
		text.setString(question.question);
		for (int i = 0; i < question.choices.size(); ++i) {
			answers.push_back(AnswerDisplay{ question.choices.at(i), font, i });
		}

		questionLeftSide.setTexture(*texture);
		questionLeftSide.setTextureRect({ {22, 0}, { 11, 34 } });
		questionLeftSide.setPosition(25.0f, 5.0f);
		questionLeftSide.setScale(3, 3);

		questionRightSide.setTexture(*texture);
		questionRightSide.setTextureRect({ {55, 0}, { 11, 34 } });
		questionRightSide.setPosition(1000.0f, 5.0f);
		questionRightSide.setScale(3, 3);

		questionMiddle.setTexture(*texture);
		questionMiddle.setTextureRect({ {108, 0}, { 41, 34 } });
		questionMiddle.setScale(23, 3);
		questionMiddle.setPosition(83.0f - 25.0f, 5.0f);
	}

	void changeQuestion(const Question& q, sf::Font* font) {
		question = q;
		text.setString(question.question);
		answers.clear();
		for (int i = 0; i < question.choices.size(); ++i) {
			answers.push_back(AnswerDisplay{ question.choices.at(i), font, i });
		}
	}

	void render(sf::RenderTarget& target) {
		target.draw(questionLeftSide);
		target.draw(questionMiddle);
		target.draw(questionRightSide);
		target.draw(text);
		for (auto& ans : answers) ans.render(target);
	}
};


bool loadDatabase(const std::string& filename, std::vector<Question>& database) {
	std::ifstream ifs{ filename };
	if (!ifs.is_open()) return false;

	for (std::string line; std::getline(ifs, line);) {
		Question question;
		question.question = line;
		for (size_t i = 0; i < 4; ++i) {
			std::string choice;
			std::getline(ifs, choice);
			question.choices.push_back(choice);
		}
		size_t ans; ifs >> ans;
		question.correctAnswer = ans;
		std::getline(ifs, line);
		std::getline(ifs, line);

		database.push_back(question);
	}
}


int main() {
	sf::RenderWindow window{ sf::VideoMode{ 1200u, 675u }, "Doki Doki Driving Club", sf::Style::Close };
	sf::Clock clock;

	sf::Font* font = new sf::Font;
	font->loadFromFile("ITC_ Korinna Normal.ttf");
	sf::Texture* texture = new sf::Texture;
	texture->loadFromFile("SpriteSheet.png");

	std::vector<Question> database;
	loadDatabase("dataset.txt", database);
	std::random_device rd;
	std::mt19937 g(rd());
	std::shuffle(database.begin(), database.end(), g);

	int index = 0;
	QuestionDisplay quest{ database.at(index), font, texture };
	
	float timer = 60;
	sf::Text timerText{ std::to_string((int)timer), *font };
	timerText.setPosition({ 1100, 50 });

	sf::Sprite rightSpotlight, leftSpotlight;
	rightSpotlight.setTexture(*texture);
	rightSpotlight.setTextureRect({ {30, 34}, {99, 47} });
	leftSpotlight = rightSpotlight;
	leftSpotlight.setPosition(50.0f, 600.0f);
	leftSpotlight.setRotation(3.14159f / 2);
	rightSpotlight.setPosition(1150.0f, 600.0f);
	rightSpotlight.setRotation(-3.14159f / 2);

	sf::Text score{ "Score: " + std::to_string(index), *font };
	score.setPosition(1000.0f, 600.0f);

	while (window.isOpen()) {
		sf::Event sfmlEvent;
		while (window.pollEvent(sfmlEvent)) {
			if (sfmlEvent.type == sf::Event::Closed) window.close();
			if (sfmlEvent.type == sf::Event::MouseButtonPressed) {
				if (sfmlEvent.mouseButton.y > 150 && sfmlEvent.mouseButton.y < 250) {
					if (quest.question.correctAnswer != 1) window.close();
				} else if (sfmlEvent.mouseButton.y > 250 && sfmlEvent.mouseButton.y < 350) {
					if (quest.question.correctAnswer != 2) window.close();
				} else if (sfmlEvent.mouseButton.y > 350 && sfmlEvent.mouseButton.y < 450) {
					if (quest.question.correctAnswer != 3) window.close();
				} else if (sfmlEvent.mouseButton.y > 450 && sfmlEvent.mouseButton.y < 550) {
					if (quest.question.correctAnswer != 4) window.close();
				}
				if (sfmlEvent.mouseButton.y > 150 && sfmlEvent.mouseButton.y < 550) {
					index++;
					quest.changeQuestion(database.at(index), font);
					score.setString("Score: " + std::to_string(index));
				}
			}
		}

		float deltaTime = clock.restart().asSeconds();
		timer -= deltaTime;
		timerText.setString(std::to_string((int)timer));

		window.clear({61, 0, 0});

		window.draw(timerText);
		window.draw(score);
		quest.render(window);

		window.display();
	}

	return 0;
}
