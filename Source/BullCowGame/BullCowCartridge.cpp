// Fill out your copyright notice in the Description page of Project Settings.
#include "BullCowCartridge.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

void UBullCowCartridge::BeginPlay() // When the game starts
{
    Super::BeginPlay();
	
	// Load Wordlist from file.
	TArray<FString> WordList;

	const FString WordListPath = FPaths::ProjectContentDir() / TEXT("WordLists/HiddenWordList.txt");
	FFileHelper::LoadFileToStringArray(WordList, *WordListPath);

	this->Isograms = GetValidWords(WordList);

	this->SetupGame();
}

void UBullCowCartridge::OnInput(const FString& PlayerInput) // When the player hits enter
{
	if (bGameOver)
	{
		ClearScreen();
		SetupGame();
	}
	else
	{
		ProcessGuess(PlayerInput);
	}

	PrintLine(PlayerInput);
}

void UBullCowCartridge::SetupGame()
{
	this->HiddenWord = this->Isograms[FMath::RandRange(0, this->Isograms.Num() - 1)];
	this->Lives = this->HiddenWord.Len() * 2;
	this->bGameOver = false;

	PrintLine(TEXT("The HiddenWord is: %s"), *HiddenWord); //Debug Line

	PrintLine(TEXT("Welcome to Bull Cows"));
	PrintLine(TEXT("Guess the %i Letter Word"), HiddenWord.Len());
	PrintLine(TEXT("Type in your guess. \nPress enter to continue..."));
}

void UBullCowCartridge::EndGame()
{
	this->bGameOver = true;

	PrintLine(TEXT("\nPress enter to play again."));
}

void UBullCowCartridge::ProcessGuess(const FString& Guess)
{
	//Check if the HiddenWord is equal to the player input 
	if (this->HiddenWord == Guess)
	{
		PrintLine(TEXT("You have Won"));
		EndGame();

		return;
	}

	//Check Right number of characters
	if (this->HiddenWord.Len() != Guess.Len())
	{
		PrintLine(TEXT("The Hidden Word is %i letters long, \nTry again"), this->HiddenWord.Len());
		PrintLine(TEXT("Sorry, try guessing again! \nYou have %i lives remaining"), this->Lives);
		
		return;
	}

	//Check if the word is not an Isogram
	if (!IsIsogram(Guess))
	{
		PrintLine(TEXT("No Repeating letters, guess again"));
		return;
	}

	//Remove Life
	--this->Lives;
	PrintLine(TEXT("You have lost a life"));

	if (this->Lives <= 0)
	{
		ClearScreen();

		PrintLine(TEXT("You have no lives left"));
		PrintLine(TEXT("The hidden word was: %s"), *this->HiddenWord);

		EndGame();
		return;
	}

	// Show the player Bulls and Cows
	FBullCowCount Score = GetBullCows(Guess);
	PrintLine(TEXT("You have %i Bulls and %i Cows"), Score.Bulls, Score.Cows);
	PrintLine(TEXT("Sorry, try guessing again! \nYou have %i lives remaining"), this->Lives);

}

bool UBullCowCartridge::IsIsogram(const FString& Word) const
{
	for (int32 i = 0; i < Word.Len(); i++)
	{
		for (int32 n = i + 1; n < Word.Len(); n++)
		{
			if (Word[i] == Word[n])
			{
				return false;
			}
		}
	}
		
	return true;
}

TArray<FString> UBullCowCartridge::GetValidWords(const TArray<FString>& WordList) const
{
	TArray<FString> ValidWords;
	// Remove Words that is not an isogram
	for (FString Word : WordList)
	{
		if (Word.Len() >= 4 && Word.Len() <= 8 && IsIsogram(Word))
		{
			ValidWords.Emplace(Word);
		}
	}
	return ValidWords;
}

FBullCowCount UBullCowCartridge::GetBullCows(const FString& Guess) const
{
	FBullCowCount Count;

	for (int32 GuessIndex = 0; GuessIndex < Guess.Len(); GuessIndex++)
	{
		if (Guess[GuessIndex] == HiddenWord[GuessIndex])
		{
			Count.Bulls++;
			continue;
		}
		for (int32 HiddenIndex = 0; HiddenIndex < HiddenWord.Len(); HiddenIndex++)
		{
			if (Guess[GuessIndex] == HiddenWord[HiddenIndex])
			{
				Count.Cows++;
				break;
			}
		}
	}
	return Count;
}