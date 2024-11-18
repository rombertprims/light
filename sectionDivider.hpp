#pragma once

#include <vector>

struct Section{
    int FirstLed;
    int LastLed;
};

class SectionDivider {
public:

 SectionDivider();

    void addSection(Section section);  //functies benoenoemen dat ze bestaan zodat het programma weet wat er gebeurd.
    void addSectionsSquare(int sideLength); //Voor de init van de sectionDivider

    int getOppositeSectionIndex(int sectionIndex);

    Section getSquare(int squareIndex);                 //si=0 buitenste ring, si=3 binneste ring
    Section getSide(int sectionIndex);
    Section getSide(int squareIndex, int sideIndex);    //return 1 section
    Section getPhaseFromMiddle(double phase, Section section);  //Return gedeelte van section
    int getLedsPerRing(int squareIndex);

    std::vector<Section> getSectionsPhaseFromMiddle(double phase, std::vector<Section> sections);
    std::vector<Section> getSections();                 //Return alle zijdes
    std::vector<Section> getVectorSquare(int squareIndex);   //Return square in de vorm van 4 vectors
    std::vector<Section> getSides(int sideIndex);           //Return l/r/boven/onder van alle squares.
    std::vector<Section> getOppositeSides(int sectionIndex);
    std::vector<Section> getSubSections(int numberSubSections);


    

private:
    RGBA* mLedArray;
    std::vector<Section> mSections;
    int ledsPerSquare[4]={160,120,100,80};


};
//Constructor
SectionDivider::SectionDivider() //init van de class--> hiermee wordt object dus aangemaakt.
{
    addSectionsSquare(40);
    addSectionsSquare(30);
    addSectionsSquare(25);
    addSectionsSquare(20);
}

void SectionDivider::addSection(Section section) { // section wordt toegevoegd. Als method moeten we dan dus een section meegeven.
    mSections.emplace_back(section);
}

void SectionDivider::addSectionsSquare(int sideLength) {
    for (int i = 0; i < 4; i++)
    {
        if (mSections.empty()) {
            addSection(Section{0,sideLength-1});
        } else {
        addSection(Section{(mSections.back().LastLed+1), (mSections.back().LastLed+sideLength)});
        }
    }
}

std::vector<Section> SectionDivider::getSections() {
    return mSections;
}

Section SectionDivider::getSquare(int i) {

    // Calculate the index range for the square
    int firstSectionSquare = i * 4;          
    int lastSectionSquare = (i + 1) * 4 - 1;

    // Get the FirstLed of the starting section and LastLed of the ending section in this square
    int sectionFirstLed = mSections[firstSectionSquare].FirstLed;
    int sectionLastLed = mSections[lastSectionSquare].LastLed;

    // Return the constructed Section
    return Section{sectionFirstLed, sectionLastLed};
}

Section SectionDivider::getSide(int sectionIndex){
    if (mSections.size()<sectionIndex){
        return Section{0,0};
    }
    return mSections[sectionIndex];
}

Section SectionDivider::getSide(int squareIndex, int sideIndex){
    // return de boven/onder/links/rechts, van een van een specifieke square
    return SectionDivider::getSide(squareIndex*4+sideIndex);
}

std::vector<Section> SectionDivider::getVectorSquare(int squareIndex){
    // return de zijkanten van een square als vector met 4 sections.
    std::vector<Section> returnSections;
    for (size_t i = 0; i < 4; i++)
    {
        returnSections.emplace_back(mSections[(squareIndex*4)+i]);
    }
    return returnSections;
    
}

std::vector<Section> SectionDivider::getSides(int sideIndex){
    //Return een vector van alle specifieke zijkanten
    std::vector<Section> returnSections;
    for (int i=0; i<4; i++){ //4 gehardcode om er voor het gemak vanuit te gaan dat we 4 squares hebben. Voor het gemak gehardcode
        returnSections.emplace_back(mSections[(i*4)+sideIndex]);
    }
    return returnSections;
}

Section SectionDivider::getPhaseFromMiddle(double phase, Section section){
    int middleLed=(section.FirstLed+section.LastLed)/2;
    if (phase>=1) phase=1;
    int firstLedPhased = Utils::rescale(phase, middleLed, section.FirstLed, 0, 1);
    int lastLedPhased = Utils::rescale(phase, middleLed, section.LastLed, 0, 1);
    return Section{firstLedPhased,lastLedPhased};

}

std::vector<Section> SectionDivider::getSectionsPhaseFromMiddle(double phase, std::vector<Section> sections){
    std::vector<Section> returnSections;
    for (auto section : sections)
    {
        returnSections.emplace_back(SectionDivider::getPhaseFromMiddle(phase, section));
    }
    return returnSections;
}

std::vector<Section> SectionDivider::getOppositeSides(int sectionIndex){
    std::vector<Section> returnSections;
    int sideIndex=sectionIndex%4;
    int ringIndex=(sectionIndex-sideIndex)/4;
    returnSections.emplace_back(SectionDivider::getSide(ringIndex, sideIndex));
    returnSections.emplace_back(SectionDivider::getSide(ringIndex, (sideIndex+2)%4));
    return returnSections;
}

int SectionDivider::getOppositeSectionIndex(int sectionIndex){
    int sideIndex=sectionIndex%4;
    int ringIndex=(sectionIndex-sideIndex)/4;
    int oppositeIndex=(sideIndex+2)%4;
    return (ringIndex*4)+oppositeIndex;
}

int SectionDivider::getLedsPerRing(int squareIndex){
    return ledsPerSquare[squareIndex];
}

std::vector<Section> SectionDivider::getSubSections(int numberSubSections) {
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

