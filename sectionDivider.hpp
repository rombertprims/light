#pragma once

#include <vector>

struct Section{
    int FirstLed;
    int LastLed;
};

class SectionDivider {
public:

 SectionDivider();

    void AddSection(Section section);  //functies benoenoemen dat ze bestaan zodat het programma weet wat er gebeurd.
    void AddSectionsSquare(int sideLength); //Voor de init van de sectionDivider

    int GetOppositeSectionIndex(int sectionIndex);

    Section GetSquare(int squareIndex);                 //si=0 buitenste ring, si=3 binneste ring
    Section GetSide(int squareIndex, int sideIndex);    //return 1 section
    Section GetPhaseFromMiddle(double phase, Section section);  //Return gedeelte van section
    int GetLedsPerRing(int squareIndex);

    std::vector<Section> GetSectionsPhaseFromMiddle(double phase, std::vector<Section> sections);
    std::vector<Section> GetSections();                 //Return alle zijdes
    std::vector<Section> GetVectorSquare(int squareIndex);   //Return square in de vorm van 4 vectors
    std::vector<Section> GetSides(int sideIndex);           //Return l/r/boven/onder van alle squares.
    std::vector<Section> GetOppositeSides(int sectionIndex);
    std::vector<Section> GetSubSections(int numberSubSections);


    

private:
    RGBA* mLedArray;
    std::vector<Section> mSections;
    int ledsPerSquare[4]={160,120,100,80};


};
//Constructor
SectionDivider::SectionDivider() //init van de class--> hiermee wordt object dus aangemaakt.
{
    AddSectionsSquare(40);
    AddSectionsSquare(30);
    AddSectionsSquare(25);
    AddSectionsSquare(20);
}

void SectionDivider::AddSection(Section section) { // section wordt toegevoegd. Als method moeten we dan dus een section meegeven.
    mSections.emplace_back(section);
}

void SectionDivider::AddSectionsSquare(int sideLength) {
    for (int i = 0; i < 4; i++)
    {
        if (mSections.empty()) {
            AddSection(Section{0,sideLength-1});
        } else {
        AddSection(Section{(mSections.back().LastLed+1), (mSections.back().LastLed+sideLength)});
        }
    }
}

std::vector<Section> SectionDivider::GetSections() {
    return mSections;
}

Section SectionDivider::GetSquare(int i) {

    // Calculate the index range for the square
    int firstSectionSquare = i * 4;          
    int lastSectionSquare = (i + 1) * 4 - 1;

    // Get the FirstLed of the starting section and LastLed of the ending section in this square
    int sectionFirstLed = mSections[firstSectionSquare].FirstLed;
    int sectionLastLed = mSections[lastSectionSquare].LastLed;

    // Return the constructed Section
    return Section{sectionFirstLed, sectionLastLed};
}

Section SectionDivider::GetSide(int squareIndex, int sideIndex){
    // return de boven/onder/links/rechts, van een van een specifieke square
    if (mSections.size()<(squareIndex*4+sideIndex))
        return Section{0,0};

    return mSections[squareIndex*4+sideIndex];
}

std::vector<Section> SectionDivider::GetVectorSquare(int squareIndex){
    // return de zijkanten van een square als vector met 4 sections.
    std::vector<Section> returnSections;
    for (size_t i = 0; i < 4; i++)
    {
        returnSections.emplace_back(mSections[(squareIndex*4)+i]);
    }
    return returnSections;
    
}

std::vector<Section> SectionDivider::GetSides(int sideIndex){
    //Return een vector van alle specifieke zijkanten
    std::vector<Section> returnSections;
    for (int i=0; i<4; i++){ //4 gehardcode om er voor het gemak vanuit te gaan dat we 4 squares hebben. Voor het gemak gehardcode
        returnSections.emplace_back(mSections[(i*4)+sideIndex]);
    }
    return returnSections;
}

Section SectionDivider::GetPhaseFromMiddle(double phase, Section section){
    int middleLed=(section.FirstLed+section.LastLed)/2;
    if (phase>=1) phase=1;
    int firstLedPhased = Utils::rescale(phase, middleLed, section.FirstLed, 0, 1);
    int lastLedPhased = Utils::rescale(phase, middleLed, section.LastLed, 0, 1);
    return Section{firstLedPhased,lastLedPhased};

}

std::vector<Section> SectionDivider::GetSectionsPhaseFromMiddle(double phase, std::vector<Section> sections){
    std::vector<Section> returnSections;
    for (auto section : sections)
    {
        returnSections.emplace_back(SectionDivider::GetPhaseFromMiddle(phase, section));
    }
    return returnSections;
}

std::vector<Section> SectionDivider::GetOppositeSides(int sectionIndex){
    std::vector<Section> returnSections;
    int sideIndex=sectionIndex%4;
    int ringIndex=(sectionIndex-sideIndex)/4;
    returnSections.emplace_back(SectionDivider::GetSide(ringIndex, sideIndex));
    returnSections.emplace_back(SectionDivider::GetSide(ringIndex, (sideIndex+2)%4));
    return returnSections;
}

int SectionDivider::GetOppositeSectionIndex(int sectionIndex){
    int sideIndex=sectionIndex%4;
    int ringIndex=(sectionIndex-sideIndex)/4;
    int oppositeIndex=(sideIndex+2)%4;
    return (ringIndex*4)+oppositeIndex;
}

int SectionDivider::GetLedsPerRing(int squareIndex){
    return ledsPerSquare[squareIndex];
}

std::vector<Section> SectionDivider::GetSubSections(int numberSubSections) {
    std::vector<Section> result;

    for (const Section& section : mSections) {
        int totalLeds = section.LastLed - section.FirstLed + 1;
        
        // Base number of LEDs per subsection
        int ledsPerSubSection = totalLeds / numberSubSections;
        
        // Calculate how many subsections need an extra LED
        int remainder = totalLeds % numberSubSections;

        int start = section.FirstLed;
        
        for (int i = 0; i < numberSubSections; ++i) {
            int end = start + ledsPerSubSection - 1;
            
            // Distribute remainder LEDs across the first few subsections
            if (remainder > 0) {
                end += 1;
                remainder -= 1;
            }

            // Ensure we don't exceed the section's LastLed
            if (end > section.LastLed) {
                end = section.LastLed;
            }

            result.push_back({start, end});
            start = end + 1;
        }
    }

    return result;
}

