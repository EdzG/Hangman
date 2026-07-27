
CREATE DATABASE hangman;
USE hangman;

CREATE TABLE wordSet(
setId INT(6) NOT NULL PRIMARY KEY AUTO_INCREMENT,
setName VARCHAR(20) NOT NULL
);

INSERT INTO wordSet(setName)
VALUES ('computer science'); 

CREATE TABLE Words (
    wordId INT NOT NULL AUTO_INCREMENT,
    wordName VARCHAR(255) NOT NULL,
    definition TEXT(255) NOT NULL, 
	setID INT(6),
    PRIMARY KEY (wordId),
    FOREIGN KEY (setId) REFERENCES wordSet(setId)
);


INSERT INTO Words (wordName, definition, setId)
Values ('Computer Science', 'The scientific study of computation, applied to both hardware and software, covering both theoretical and practical concerns.', '1'),
('complexity', 'The way that a solution to a problem scales as the size of the input increases, considering both the number of computational steps and the memory space required.', '1'),
('Browser', 'A piece of software that enables a user to locate, retrieve and display information on the world wide web.', '1'),
('Function', 'A small section of computational code that performs a specific operation. In particular, a function takes inputs, or arguments, and returns outputs, or results.', '1');



ALTER TABLE wordSet
modify numOfWords INT NOT NULL;

UPDATE wordSet
SET numOfWords = 4
WHERE setId = 1;

