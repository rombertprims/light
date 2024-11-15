#pragma once
#include "core/generation/patterns/pattern.hpp"
#include "generation/patterns/helpers/fade.h"
#include "generation/patterns/helpers/interval.h"
#include "generation/patterns/helpers/timeline.h"
#include "generation/pixelMap.hpp"
#include <math.h>
#include <vector>
#include "sectionDivider.hpp"

namespace ExamplePatterns
{

    //////////////////
    // Basics
    //////////////////

    class HelloWorld : public Pattern<RGBA>
    {
        // This example will paint all leds red. It will show you the minimal code you need in a pattern

    public:
        HelloWorld()
        {
            // The name is displayed in the controller
            this->name = "Hello world";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            // ControlHubInput will run this Calculate method on patterns it knows about.
            // If the active parameter is false, this pattern should not be displayed.
            // That is why we immediately return here. In some examples below you will see
            // how we can use this to create fade-outs or wait for an animation to finish
            // before stopping the rendering. (eg FadeFinishPattern))
            if (!active)
                return;

            // loop trough all leds and make them red.
            for (int i = 0; i < width; i++)
                pixels[i] = RGBA(255, 0, 0, 255);

            // std::vector<Section> sections = sectionDivider.GetSections();

            // for (auto section : sections) {
            //     for (int i = section.firstLed; i <= section.lastLed; i++)
            //         pixels[i] = RGBA(255, 0, 0, 255);

            // }

        }
    };

    class Palette : public Pattern<RGBA>
    {
    public:
        Palette()
        {
            this->name = "Palette colours";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!active)
                return;

            // You can access the colours in the selected palette though the params argument.
            for (int i = 0; i < width; i++)
            {
                if (i%3==0)
                    pixels[i]=params->getPrimaryColour();
                else if (i%3==1)
                    pixels[i]=params->getSecondaryColour();
                else
                    pixels [i]=params->getHighlightColour();


                // if (i < width / 3)
                //     pixels[i] = params->getPrimaryColour();
                // else if (i < 2 * width / 3)
                //     pixels[i] = params->getSecondaryColour();
                // else
                //     pixels[i] = params->getHighlightColour();
            }
        }
    };

    class PaletteGradient : public Pattern<RGBA>
    {
    public:
        PaletteGradient()
        {
            this->name = "Palette gradient";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!active)
                return;

            // You can also access the gradient. It needs a parameter in the range 0-255
            for (int i = 0; i < width; i++)
            {
                pixels[i] = params->getGradient(255 * i / width);
            }
        }
    };

    class Blending : public Pattern<RGBA>
    {
    public:
        Blending()
        {
            this->name = "Color blending";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!active)
                return;

            RGBA red = RGBA(255, 0, 0, 255);
            RGBA blue = RGBA(0, 0, 255, 255);

            for (int i = 0; i < width; i++)
            {
                // Alpha blending is really easy. You can just multiply an RGBA color with a float to
                // make it partially transparent.
                // Here i use a base of red, with a blue layer on top of it that fades from transparent
                // to fully opaque.
                pixels[i] = red + blue * (float(i) / width);
            }
        }
    };

    class PatternLayering : public Pattern<RGBA>
    {
    public:
        PatternLayering()
        {
            this->name = "Pattern layering";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!active)
                return;

            for (int i = 0; i < width; i++)
            {
                // Alpha blending is also useful to stack multiple patterns.
                // This example applies a similar alpha gradient as the example above,
                // but this time without a background color. It will blend with the
                // pattern underneath it.
                // The effect is clearly visible if you enable 'Hello world' and
                // this pattern 'Pattern Layering' together.
                pixels[i] = params->getHighlightColour() * (float(i) / width);

                // The pixels[] array is filled with RGBA(0,0,0,0) (i.e.: all pixels transparent)
                // before this Calculate function is called. That is why you can return early if
                // active is false. It also means that you only need to paint the pixels that
                // your pattern covers.
            }
        }
    };

    //////////////////
    // LFO
    //////////////////

    // Now you know the basics, it is up to you how you want to fill the array of pixel data.
    // Everything is allowed. You can make your own code a simple or complex as you want.
    // There are a few helper classes that you can use if you like. They save you from writing
    // the same logic over and over again.
    // Below are a few examples of the most used helpers

    class LFOPattern : public Pattern<RGBA>
    {
    public:
        // Here the lfo object is created.
        // Different shapes are available. Common ones are:
        // Sin:             Speaks for itself
        // SinFast:         Since calculating sin() is relatively slow, this shape uses a lookup table with 255 precalculated values.
        // NegativeCosFast: Similar to sin, but has its starting point at the bottom instead of the center. Also uses lookup table.
        // SawDown:         Goes from 1 to 0, linearly.
        LFO<SinFast> lfo;

        LFOPattern()
        {
            this->name = "LFO";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!active)
                return;

            for (int i = 0; i < width; i++)
            {
                // lfo.getValue() will give you the value in the range 0-1.
                // This is convenient when you want to do alpha blending,
                pixels[i] = params->getPrimaryColour() * lfo.getValue();
            }
        }
    };

    class LFOChase : public Pattern<RGBA>
    {
    public:
        // LFO is suited to create chases.
        // In this example i use SawDown, but you can play around, and try different shapes
        //LFO<SawDown> lfo;
        SectionDivider sectionDivider;
        LFOTempo<SawDown> lfo;
        //Deze gaat mis als je switcht van van constant tempo naar ander tap/tempo.


        LFOChase()
        {
            this->name = "LFO chase";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!active)
                return;

            lfo.setPeriod(4);

            // int startLed=0;
            // for (int i = 0; i < 4; i++)
            // {
            //     for (int j = startLed; j < startLed+sectionDivider.GetLedsPerRing(i); j++)
            //     {
            //         float phase = float(j-startLed) / sectionDivider.GetLedsPerRing(i);
            //         pixels[j] = params->getPrimaryColour() * lfo.getValue(phase);
            //     }
            //     startLed=startLed+sectionDivider.GetLedsPerRing(i);
            // }

            for (int i = 0; i < 4; i++)
            {
                for (int j = sectionDivider.GetSquare(i).FirstLed; j <= sectionDivider.GetSquare(i).LastLed; j++)
                {
                    float phase=float(j-sectionDivider.GetSquare(i).FirstLed)/sectionDivider.GetLedsPerRing(i);
                    pixels[j]=params->getPrimaryColour()*lfo.getValue(phase);
                }
            }
        }
    };

    class LFOChaseMasked : public Pattern<RGBA>
    {
    public:
        // LFO is suited to create chases.
        // In this example i use SawDown, but you can play around, and try different shapes
        LFO<PWM> lfo;
        LFO<PWM> lfo2;
        SectionDivider sectionDivider;
        Permute permute;
        BeatWatcher watcher;

        LFOChaseMasked()
        {
            this->name = "LFO chaseMasked";
        }
        

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!active)
                return;
            
            lfo.setDutyCycle(0.25);
            lfo2.setDutyCycle(0.25);

            if (Tempo::TimeBetweenBeats()>100){
                lfo.setPeriod(4*Tempo::TimeBetweenBeats());
                lfo2.setPeriod(2*Tempo::TimeBetweenBeats());
            }
        
            if (watcher.Triggered()){
                permute.permute();
            }

            for (int i = 0; i < 4; i++)
            {
                
                int square=i;

                for (int j = sectionDivider.GetSquare(square).FirstLed; j <= sectionDivider.GetSquare(square).LastLed; j++)
                {
                    float phase=float(j-sectionDivider.GetSquare(square).FirstLed)*4/sectionDivider.GetLedsPerRing(square);
                    pixels[j]=params->getPrimaryColour()*lfo.getValue(phase+0.1)+params->getSecondaryColour()*lfo.getValue(-phase*1.2);
                    if (i==2){
                        pixels[j]=params->getPrimaryColour()*lfo2.getValue(phase+0.1)+params->getSecondaryColour()*lfo2.getValue(-phase*1.2);
                    }
                }
            }

            // for (int i = 0; i < params->getAmount(1,4); i++)
            // {
            //     int square=permute.at[i];

            //     for (int j = sectionDivider.GetSquare(square).FirstLed; j <= sectionDivider.GetSquare(square).LastLed; j++)
            //     {
            //         float phase=float(j-sectionDivider.GetSquare(square).FirstLed)*4/sectionDivider.GetLedsPerRing(square);
            //         pixels[j]=params->getPrimaryColour()*lfo.getValue(-phase);
            //     }
            // }    
            
        }
    };


    class LFOChaseGradient : public Pattern<RGBA>
    {
    public:
        // Example with lfo gradient
        LFO<SawDown> lfo;

        LFOChaseGradient()
        {
            this->name = "LFO chase gradient";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!active)
                return;

            for (int i = 0; i < width; i++)
            {
                float phase = float(i) / width;
                float lfoVal = lfo.getValue(phase);  //THis value is between 0-1 so makkelijke ding 
                // Here i use lfoVal to lookup the gradient value, but also to apply transparency,
                // so the chase nicely fades to transparent
                pixels[i] = params->getGradient(255 * lfoVal) * lfoVal;
            }
        }
    };

    class LFOAntiAlias : public Pattern<RGBA>
    {
    public:
        // You can use soft shapes like SoftPWM, SoftSawUp, SoftSawDown to take
        // off the hard edges when you use LFO to create a spatial wave.
        LFO<SoftPWM> lfo;

        LFOAntiAlias()
        {
            this->name = "LFO anti alias";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!active)
                return;

            lfo.setPeriod(10000);
            lfo.setDutyCycle(0.5);
            // A good value for the soft edge width is 1 pixel distance,
            // which is equal to the smallest value to pass as deltaPhase
            // to lfo.getValue(). so, in most cases: 1/width
            // Try with a higher value, eg 0.25 so see adn exaggerated effect
            lfo.setSoftEdgeWidth(3. / width);

            for (int i = 0; i < width; i++)
            {
                float phase = float(i) / width;
                pixels[i] = params->getPrimaryColour() * lfo.getValue(phase);
            }
        }
    };

    //////////////////
    // Fade
    //////////////////

    class FadePattern : public Pattern<RGBA>
    {
    public:
        // This example shows a Fade and BeatWatcher. These two helpers are typically used together.
        // FadeDown fade;
        BeatWatcher watcher;
        SectionDivider sectionDivider;
        int iteratie=0;
        RGBA colors[4]={{0, 255, 0, 255},{255, 0, 0, 255},{255, 255, 0, 255},{0, 0, 255, 255}};

        FadePattern()
        {
            this->name = "Fade";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!active)
                return;

            // Watcher.Triggered() will return true in the first frame after a beat occurred.
            // This is used to reset the fade
            if (watcher.Triggered())
                iteratie=(iteratie+1)%4;
            
            for (int i=0;i<4;i++){
                for (int j=sectionDivider.GetSquare(i).FirstLed; j<=sectionDivider.GetSquare(i).LastLed;j++)
                {
                    pixels[j]=colors[(iteratie+i)%4];
                }
            }
                //     fade.reset();

            // for (int i = 0; i < width; i++)
            // {
            //     pixels[i] = params->getPrimaryColour() * fade.getValue();
            // }
        }
    };

    class FadeChasePattern : public Pattern<RGBA>
    {
    public:
        FadeDown fade;
        BeatWatcher watcher;

        FadeChasePattern()
        {
            this->name = "Fade Chase";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!active)
                return;

            if (watcher.Triggered())
                fade.reset();

            for (int i = 0; i < width; i++)
            {
                // It is also easy to change this fade into a chase by providing an argument to getValue.
                // this parameter is the start delay in milliseconds.
                // This means that the fade will wait X milliseconds after the trigger to start fading.
                // By passing a slightly different delay for each pixel it will create a chase effect.
                float phase = ((float)i) / width;
                pixels[i] = params->getPrimaryColour() * fade.getValue(500 * phase);
            }
        }
    };

    class FadeFinishPattern : public Pattern<RGBA>
    {
    public:
        FadeDown fade;
        BeatWatcher watcher;

        FadeFinishPattern()
        {
            this->name = "Fade finish";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            float velocity = 2;

            // In this example, we wait for the fade animation to finished before stopping the pattern.
            // This makes the transition a little bit nicer.

            // only trigger a new fade if the pattern is still active
            if (active && watcher.Triggered())
                fade.reset();

            // If the fade is finished we can stop rendering
            // You need to pass the max phase delay here, so it can correct for the start of the most delayed fade.
            // in this case: velocity * width / width = velocity
            if (fade.isFinished(velocity))
                return;

            for (int i = 0; i < width; i++)
            {
                float phase = ((float)i) / width;
                pixels[i] = params->getPrimaryColour() * fade.getValue(velocity * phase);
            }
        }
    };

    //////////////////
    // Permute
    //////////////////

    class PermutePattern : public Pattern<RGBA>
    {
    public:
        // You can use permute to create patterns that apply to only some of the lights.
        // Permute will create a list of numbers in random order.
        // eg Permute with a size of 5 could give you [3,4,1,0,2]
        FadeDown fade;
        BeatWatcher watcher;
        Permute permute;

        PermutePattern()
        {
            this->name = "Permute";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!active)
                return;

            // We need to tell permute how many leds we are working with
            permute.setSize(width);

            if (watcher.Triggered())
            {
                // Reorder the permutations on each beat
                permute.permute();
                fade.reset();
            }

            // Notice that we only loop to width / 4, so we only paint 25% of the pixels
            for (int i = 0; i < width / 4; i++)
            {
                // lookup the number of the pixel we are going to paint:
                int randomizedPixelIndex = permute.at[i];
                pixels[randomizedPixelIndex] = params->getPrimaryColour() * fade.getValue();
            }
        }
    };

    class Custom1 : public Pattern<RGBA>
    {
    public:
        // You can use permute to create patterns that apply to only some of the lights.
        // Permute will create a list of numbers in random order.
        // eg Permute with a size of 5 could give you [3,4,1,0,2]
        BeatWatcher watcher;
        int ringselector=0;
        SectionDivider sectionDivider;
        FadeDown fade;

        Custom1()
        {
            this->name = "Custom1";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            
            float velocity=2;
            if (!active){
                ringselector=0;
                return;
            }
            // if (active && watcher.Triggered())
            //     fade.reset();

            // // If the fade is finished we can stop rendering
            // // You need to pass the max phase delay here, so it can correct for the start of the most delayed fade.
            // // in this case: velocity * width / width = velocity
            // if (fade.isFinished(velocity))
            //     return;

            // for (int i = 0; i < width; i++)
            // {
            //     float phase = ((float)i) / width;
            //     pixels[i] = params->getPrimaryColour() * fade.getValue(velocity * phase);
            // }

            if (active&&watcher.Triggered())
            {
                ringselector=(ringselector+1)%4;
                fade.reset();
                
            }

            if (fade.isFinished(velocity))
                return;

            for (int i=sectionDivider.GetSquare(ringselector).FirstLed; i<=sectionDivider.GetSquare(ringselector).LastLed;i++)
                {
                    pixels[i]=params->getPrimaryColour() * fade.getValue(velocity);
                }
        }
    };

    class Custom2 : public Pattern<RGBA>
    {
    public:
        // You can use permute to create patterns that apply to only some of the lights.
        // Permute will create a list of numbers in random order.
        // eg Permute with a size of 5 could give you [3,4,1,0,2]
        BeatWatcher watcher;
        SectionDivider sectionDivider;
        int sideselector=0;
        FadeDown fade;


        Custom2()
        {
            this->name = "Custom2";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!active){
                sideselector=0;
                return;
            }

            if (watcher.Triggered())
            {
                sideselector=(sideselector+1)%4;
                fade.reset();
            }

            for (auto & element : sectionDivider.GetSides(sideselector)) {
                for (int i = element.FirstLed;i<=element.LastLed;i++){
                    pixels[i]=params->getSecondaryColour()*fade.getValue();
                }
            }
        }
    };

    class Custom3 : public Pattern<RGBA>
    {
    public:
        BeatWatcher watcher;
        SectionDivider sectionDivider;
        Permute permute;
        FadeDown fade;

        Custom3()
        {
            this->name = "Custom3";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!active)
                return;

            //Set size to amount of sections
            permute.setSize(sectionDivider.GetSections().size());

            if (watcher.Triggered())
            {
                // Reorder the permutations on each beat
                permute.permute();
                fade.reset();
                
            }
            for (int j=0; j< params->getAmount(1,5); j++) {
                int randomizedSectionIndex = permute.at[j];
                if (permute.at[j + 1] % 2 == 0){
                    for (int i = sectionDivider.GetSections().at(randomizedSectionIndex).FirstLed; i<=sectionDivider.GetSections().at(randomizedSectionIndex).LastLed;i++){
                        pixels[i]=params->getPrimaryColour()* fade.getValue();
                    }
                } else{
                    for (int i = sectionDivider.GetSections().at(randomizedSectionIndex).FirstLed; i<=sectionDivider.GetSections().at(randomizedSectionIndex).LastLed;i++){
                        pixels[i]=params->getSecondaryColour()* fade.getValue();
                    }
                }
            }
        }
    };

    class Custom4 : public Pattern<RGBA>
    {
    public:
        BeatWatcher watcher;
        SectionDivider sectionDivider;
        LFO<SoftPWM> lfo;
        LFO<SinFast> sinf;
        

        Custom4()
        {
            this->name = "Custom4";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!active)
                return;
            
            lfo.setPeriod(10000);
            
            lfo.setDutyCycle(0.25* params->getAmount(1,3));           
            

            // We need to tell permute how many leds we are working with
            for (int i=0; i<4; i++){
                for (int j=sectionDivider.GetSquare(i).FirstLed; j<=sectionDivider.GetSquare(i).LastLed;j++)
                {
                    float phase = Utils::rescale(j, i*0.25, (i+1)*0.25, sectionDivider.GetSquare(i).FirstLed, sectionDivider.GetSquare(i).LastLed);

                    pixels[j]=params->getPrimaryColour()*lfo.getValue(phase);
                }

            }
            

        }
    };

    class Custom5 : public Pattern<RGBA>
    {
    public:
        SectionDivider sectionDivider;
        BeatWatcher watcher;
        int sectionIndex=0;
    

        Custom5()
        {
            this->name = "Custom5";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!active)
                return;
            int number_of_sub_sections=params->getAmount(1,10);
            std::vector<Section> sub_sections=sectionDivider.GetSubSections(number_of_sub_sections);
            if (watcher.Triggered()){
                sectionIndex++;
            }
            if (sectionIndex>=sub_sections.size()){
                sectionIndex=0;
            }
            
            for (int i = sub_sections.at(sectionIndex).FirstLed; i <= sub_sections.at(sectionIndex).LastLed; i++)
            {
                pixels[i]=params->getPrimaryColour();
            }
            

            
        
        }
    };

    class WaterDrop : public Pattern<RGBA>
    {
    public:
        SectionDivider sectionDivider;
        Permute permute;
        BeatWatcher watcher;
        int sectionIndex=0;

        WaterDrop()
        {
            this->name="WaterDrop";
        }
        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!active){
                return;
            }
            permute.setSize(16);
            if (watcher.Triggered()){
                sectionIndex++;
                if (sectionIndex>=16){
                    permute.permute();
                    sectionIndex=0;
                }
            }
            Section permutedSection = sectionDivider.GetSections().at(permute.at[sectionIndex]);
            Section phasedPermutedSection = sectionDivider.GetPhaseFromMiddle((Tempo::GetProgress(1)+0.1),permutedSection);
            for (int i = phasedPermutedSection.FirstLed; i <= phasedPermutedSection.LastLed; i++)
            {
                pixels[i]=params->getPrimaryColour();
            }

        }
    };

    class WaterDrop1 : public Pattern<RGBA>
    {
    public:
        SectionDivider sectionDivider;
        Permute permute;
        BeatWatcher watcher;
        int sectionIndex=0;

        WaterDrop1()
        {
            this->name="WaterDrop1";
        }
        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!active){
                return;
            }
            permute.setSize(16);
            if (watcher.Triggered()){
                sectionIndex++;
                if (sectionIndex>=16){
                    permute.permute();
                    sectionIndex=0;
                }
            }
            Section permutedSection = sectionDivider.GetSections().at(permute.at[sectionIndex]);
            Section phasedPermutedSection = sectionDivider.GetPhaseFromMiddle((Tempo::GetProgress(1)+0.1),permutedSection);
            for (int i = phasedPermutedSection.FirstLed; i <= phasedPermutedSection.LastLed; i++)
            {
                pixels[i]=params->getPrimaryColour();
            }

            Section permutedSection1 = sectionDivider.GetSections().at(sectionDivider.GetOppositeSectionIndex(permute.at[sectionIndex]));
            Section phasedPermutedSection1 = sectionDivider.GetPhaseFromMiddle((Tempo::GetProgress(1)+0.1),permutedSection1);
            for (int i = phasedPermutedSection1.FirstLed; i <= phasedPermutedSection1.LastLed; i++)
            {
                pixels[i]=params->getPrimaryColour();
            }

        }
    };

    class WaterDrop2 : public Pattern<RGBA>
    {
    public:
        SectionDivider sectionDivider;
        Permute permute;
        BeatWatcher watcher;
        int sectionIndex=0;
        int last_ring;
        std::vector<Section> sections;

        WaterDrop2()
        {
            this->name="WaterDrop2";
        }
        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!active){
                return;
            }
            permute.setSize(4);
            if (watcher.Triggered()){
                sectionIndex++;
                if (sectionIndex>=4){
                    last_ring=permute.at[3];
                    permute.permute();
                    if (permute.at[0]==last_ring){  //als de eerste zelfde is als eerstvolgende van de nieuwe permuted array gewoon nog een keer permuten. na twee keer permuten vind ik het genoeg.
                        permute.permute();
                    }
                    sectionIndex=0;
                }
            }
            float variant = params->getVariant();
            
            if (variant<=(1./3.)){
                sections = sectionDivider.GetVectorSquare(sectionIndex);
            } else if (variant<=(2./3.)){
                sections = sectionDivider.GetVectorSquare(permute.at[sectionIndex]);
            } else {
                sections = sectionDivider.GetVectorSquare(3-sectionIndex);
            }
                

            //std::vector<Section> sections = sectionDivider.GetVectorSquare(permute.at[sectionIndex]);
            for (auto section : sections) {
                //Section permutedSection = sectionDivider.GetVectorSquare().at(permute.at[sectionIndex]);
                Section phasedPermutedSection = sectionDivider.GetPhaseFromMiddle((Tempo::GetProgress(1)+0.1),section);
                for (int i = phasedPermutedSection.FirstLed; i <= phasedPermutedSection.LastLed; i++)
                {
                    pixels[i]=params->getPrimaryColour();
                }
            }

        }
    };

    class SubSectionStrobe : public Pattern<RGBA>
    {
    public:
        SectionDivider sectionDivider;
        Timeline timeline = Timeline(50);
        Permute perm;
    

        SubSectionStrobe()
        {
            this->name = "SubSectionStrobe";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!active)
                return;
            int number_of_sub_sections=10;
            
            std::vector<Section> sub_sections=sectionDivider.GetSubSections(number_of_sub_sections);
            
            timeline.FrameStart();
            perm.setSize(sub_sections.size());

            if (timeline.Happened(0))
                perm.permute();
            
            float amount_sub_sections_lit_up=params->getAmount(0.,0.20);
            for (int i = 0; i < sub_sections.size()*amount_sub_sections_lit_up; i++)
            {
                for (int j = sub_sections.at(perm.at[i]).FirstLed; j <= sub_sections.at(perm.at[i]).LastLed; j++)
                {
                    pixels[j]=RGBA(255,255,255,255);
                }
                
            }

            
        
        }
    };
   
   class MeesterPlusser : public Pattern<RGBA>
    {
        // This example will paint all leds red. It will show you the minimal code you need in a pattern

    public:

        BeatWatcher watcher;
        SectionDivider sd;
        int horizontal[4]={12,8,4,0};
        int vertical[4]={13,9,5,1};
        //int current[4]={0,0,0,0};
        bool orientation=true;

        MeesterPlusser()
        {
            // The name is displayed in the controller
            this->name = "MeesterPlusser";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {

            if (!active)
                return;

            // if (watcher.Triggered()){
                
            // }
            orientation = (Tempo::GetProgress(8) >=0.5);
            int index = 4*Tempo::GetProgress(4);
            for (int j = 0; j <= index; j++)
            {
                int sideIndex;
                if (orientation){
                    sideIndex=horizontal[j];
                } else {
                    sideIndex=vertical[j];
                }
                for (int i = sd.GetOppositeSides(sideIndex).at(0).FirstLed; i <= sd.GetOppositeSides(sideIndex).at(0).LastLed; i++)
                {
                    pixels[i]=params->getPrimaryColour();
                }
                for (int i = sd.GetOppositeSides(sideIndex).at(1).FirstLed; i <= sd.GetOppositeSides(sideIndex).at(1).LastLed; i++)
                {
                    pixels[i]=params->getPrimaryColour();
                }
            }
            
            
            
            



        }
    };
    //////////////////
    // Params   
    //////////////////

    class ParamsPattern : public Pattern<RGBA>
    {
    public:
        LFO<PWM> lfo;

        ParamsPattern()
        {
            this->name = "Params";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!active)
                return;

            // You can use params in you patterns to give the user freedom to further tweak some value.
            // I use a predetermined set of params (Velocity, amount, size, etc).
            // You can get their values from the params argument. By default they will be in the range 0-1,
            // but you can rescale it by providing a min and max argument.

            lfo.setPeriod(params->getVelocity(5000, 500));
            lfo.setDutyCycle(params->getSize(0.1, 0.9));
            int amount = params->getAmount(1, 4);

            for (int i = 0; i < width; i++)
            {
                float phase = float(i) / width;
                pixels[i] = params->getPrimaryColour() * lfo.getValue(amount * phase);
            }
        }
    };

    class LFOGlow : public Pattern<RGBA>
    {
    public:
        // NegativeCosFast works nice in combination with duty cycle.
        // So nice that i gave it an alias: Glow
        LFO<Glow> lfo;
        // Use permute to randomize the pixel order
        Permute permute;

        LFOGlow()
        {
            this->name = "LFO glow";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!active)
                return;

            permute.setSize(width);

            lfo.setPeriod(params->getVelocity(5000, 500));
            lfo.setDutyCycle(params->getAmount(0.1, 1));

            for (int i = 0; i < width; i++)
            {
                float phase = float(i) / width;
                int randomizedPixelIndex = permute.at[i];
                pixels[randomizedPixelIndex] = params->getPrimaryColour() * lfo.getValue(phase);
            }
        }
    };

    class FadeChase : public Pattern<RGBA>
    {
    public:
        FadeDown fade;
        BeatWatcher watcher;

        FadeChase()
        {
            this->name = "Fade chase";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!active)
                return;

            if (watcher.Triggered())
                fade.reset();

            // When using fade to create spatial patterns, you can use setChaseLength
            // to help you set the duration.
            // First calculate the max startDelay based on the velocity.
            // maxDelay is used in the argument of fade.getValue(), but also in setChaseLength.
            // Because we have an array of pixels that start one after another, we visualize this
            // as a moving chase. The length of the tail is determined by the time it takes the
            // fade to fade out and the amount the chase has 'moved forward'.
            // The tail length is therefore dependent on maxDelay and the duration.
            float maxDelay = params->getVelocity(2000, 500);
            fade.setTailLength(params->getSize(0, 2), maxDelay);

            for (int i = 0; i < width; i++)
            {
                float phase = float(i) / width;
                pixels[i] = params->getPrimaryColour() * fade.getValue(phase * maxDelay);
            }
        }
    };

    class StrobeHighlightPattern : public Pattern<RGBA>
    {
        int framecounter = 1;

    public:
        StrobeHighlightPattern()
        {
            this->name = "Strobe white";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!active)
                return;

            framecounter--;

            RGBA color = RGBA(0, 0, 0, 255);
            if (framecounter <= 1)
                color = params->getHighlightColour();

            if (framecounter == 0)
                framecounter = 5; // params->getVelocity(40,4);

            for (int index = 0; index < width; index++)
                pixels[index] = color;
        }
    };

    class PixelGlitchPattern : public Pattern<RGBA>
    {
        Timeline timeline = Timeline(500);
        Permute perm = Permute(0);
        Transition transition = Transition(
            200, Transition::none, 0,
            1000, Transition::none, 0);

    public:
        PixelGlitchPattern()
        {
            this->name = "Pixel glitch";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            timeline.FrameStart();
            perm.setSize(width);

            if (timeline.Happened(0))
                perm.permute();

            for (int index = 0; index < width / 30; index++)
                pixels[perm.at[index]] = params->getSecondaryColour() * transition.getValue();
        }
    };

    class DoubleFlash : public Pattern<RGBA>
    {
        Timeline timeline = Timeline();
        BeatWatcher watcher;

    public:
        DoubleFlash()
        {
            this->name = "Double flash";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!active)
                return;

            timeline.FrameStart();

            int beatDiv = params->getAmount(0,4);
            if (beatDiv >3) beatDiv =3;
            int divs[] = {8,4,2,1};

            if (watcher.Triggered() && Tempo::GetBeatNumber() % divs[beatDiv] == 0)
                timeline.reset();

            if (!timeline.Happened(0) && !timeline.Happened(100))
                return;

            for (int i = 0; i < width; i++)
            {
                // if (map->z(i) < 0.44)
                //     continue;
                pixels[i] = params->getHighlightColour();
            }
        }
    };

    //////////////////
    // Mapping
    //////////////////

    class MappedPattern : public Pattern<RGBA>
    {
    public:
        LFO<SinFast> lfo;
        PixelMap *map;

        // For mapped patterns, the constructor needs to receive the map and store it in a property
        MappedPattern(PixelMap *map)
        {
            this->name = "MappedSinfast";
            this->map = map;
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!active)
                return;
            lfo.setPeriod(params->getVelocity(5000, 500));
            lfo.setDutyCycle(params->getAmount(0.1, 1));
            lfo.setSoftEdgeWidth(params->getSize(0,0.4));

            for (int i = 0; i < width; i++)
            {
                // In your pattern you can access the coordinates with map->x(i) and map->y(i)
                // You should generate your maps in range [-1,1].
                // In this example i need a phase in range [0,1], so i need to rescale the value.
                // For here on you can use it the same way you did before
                
                // float phase = Utils::rescale(map->x(i), 0, 1, -1, 1);
                // pixels[i] = params->getPrimaryColour() * lfo.getValue(phase);
                
                //float x_phase = Utils::rescale(map->x(i), 0, 1, -1, 1);
                //float y_phase = Utils::rescale(map->y(i), 0, 1, -1, 1);
                float x_phase = map->x(i);
                float y_phase = map->y(i);
                // Calculate the distance from the center (0, 0) in both x and y directions.
                float phase = sqrt(x_phase * x_phase + y_phase * y_phase);

                // Use the phase for the LFO value.
                pixels[i] = params->getPrimaryColour() * lfo.getValue(phase);
            }
        }
    };

    class MappedPattern1 : public Pattern<RGBA>
    {
    public:
        LFO<SawDown> lfo;
        PixelMap *map;
        LFO<NegativeCosFast> lfoCos;
        LFO<PWM> lfoPWM;
        BeatWatcher watcher;

        // For mapped patterns, the constructor needs to receive the map and store it in a property
        MappedPattern1(PixelMap *map)
        {
            this->name = "MapSawDown";
            this->map = map;
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!active)
                return;
            lfo.setPeriod(params->getVelocity(5000, 500));
            lfo.setDutyCycle(params->getAmount(0.1, 1));
            lfo.setSoftEdgeWidth(params->getSize(0,0.4));
            lfoCos.setPeriod(1000);
            float variant=params->getVariant();
            float offset=params->getOffset();
            lfoPWM.setDutyCycle(0.25);
            lfoPWM.setPeriod(500);

            if (watcher.Triggered()){
                lfoPWM.reset();
            }

            for (int i = 0; i < width; i++)
            {
                // In your pattern you can access the coordinates with map->x(i) and map->y(i)
                // You should generate your maps in range [-1,1].
                // In this example i need a phase in range [0,1], so i need to rescale the value.
                // For here on you can use it the same way you did before
                
                // float phase = Utils::rescale(map->x(i), 0, 1, -1, 1);
                // pixels[i] = params->getPrimaryColour() * lfo.getValue(phase);
                
                //float x_phase = Utils::rescale(map->x(i), 0, 1, -1, 1);
                //float y_phase = Utils::rescale(map->y(i), 0, 1, -1, 1);
                float phase;
                
                if (variant<0.5){
                    phase=(map->x(i)+offset*map->y(i))/(1+offset);
                } else {
                    phase=map->y(i);
                }

                // float x_phase = map->x(i);
                // float y_phase = map->y(i);
                // // Calculate the distance from the center (0, 0) in both x and y directions.
                // float phase = sqrt(x_phase * x_phase + y_phase * y_phase);

                // Use the phase for the LFO value.
                pixels[i] = params->getPrimaryColour() * lfoPWM.getValue(phase);
            }
        }
    };

    class MappedPattern2 : public Pattern<RGBA>
    {
    public:
        LFO<SoftSawDown> lfo;
        PixelMap *map;

        // For mapped patterns, the constructor needs to receive the map and store it in a property
        MappedPattern2(PixelMap *map)
        {
            this->name = "SoftMapSawdown";
            this->map = map;
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!active)
                return;
            lfo.setPeriod(params->getVelocity(5000, 500));
            lfo.setDutyCycle(params->getAmount(0.1, 1));
            lfo.setSoftEdgeWidth(params->getSize(0,0.4));

            for (int i = 0; i < width; i++)
            {
                // In your pattern you can access the coordinates with map->x(i) and map->y(i)
                // You should generate your maps in range [-1,1].
                // In this example i need a phase in range [0,1], so i need to rescale the value.
                // For here on you can use it the same way you did before
                
                // float phase = Utils::rescale(map->x(i), 0, 1, -1, 1);
                // pixels[i] = params->getPrimaryColour() * lfo.getValue(phase);
                
                //float x_phase = Utils::rescale(map->x(i), 0, 1, -1, 1);
                //float y_phase = Utils::rescale(map->y(i), 0, 1, -1, 1);
                float x_phase = map->x(i);
                float y_phase = map->y(i);
                // Calculate the distance from the center (0, 0) in both x and y directions.
                float phase = sqrt(x_phase * x_phase + y_phase * y_phase);

                // Use the phase for the LFO value.
                pixels[i] = params->getPrimaryColour() * lfo.getValue(phase);
            }
        }
    };
   
   
    //////////////////
    // Transitions
    //////////////////

    class TransitionPattern : public Pattern<RGBA>
    {
        // Transitions are a nice way to fade-in and fade-out patterns.
        // Here i add a transition to the pattern. It has a fade in time of 500 and fade out time of 1000
        Transition transition = Transition(500, 1000);

    public:
        TransitionPattern()
        {
            this->name = "Transition";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            // transition.Calculate must be called each time. It will do some internal calculations to
            // calculate the fade-in/out position. It will return a boolean that indicates if the
            // fade is finished and the pattern should render or not.
            // This logic replaces the if(!active) return; logic that we used before.
            if (!transition.Calculate(active))
                return;

            for (int i = 0; i < width; i++)
            {
                // multiply your pixels colors with transition.getValue() to apply the fade in and out:
                pixels[i] = params->getSecondaryColour() * transition.getValue();
            }
        }
    };

    class SpatialTransitionPattern : public Pattern<RGBA>
    {
        // Transition can also have a spatial effect. In other words: start fading earlier at first pixel, and
        // start all the other pixels with a slight delay.
        // There are a couple of effects: fromStart, fromEnd, fromCenter and fromSides
        Transition transition = Transition(
            500, Transition::fromStart, 500,
            1000, Transition::fromEnd, 500);

    public:
        SpatialTransitionPattern()
        {
            this->name = "Spatial transition";
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            for (int i = 0; i < width; i++)
            {
                // pass i and width arguments to transition.getValue to tell the transition which pixel we are rendering
                pixels[i] = params->getSecondaryColour() * transition.getValue(i, width);
            }
        }
    };

    class MappedSpatialTransitionPattern : public Pattern<RGBA>
    {
        // You can also use the transition effect to create a spatial effect in
        // x,y coordinates instead of pixel index
        Transition transition = Transition(
            200, Transition::fromStart, 500,
            200, Transition::fromStart, 500);
        PixelMap *map;

    public:
        MappedSpatialTransitionPattern(PixelMap *map)
        {
            this->name = "Mapped spatial transition";
            this->map = map;
        }

        inline void Calculate(RGBA *pixels, int width, bool active, Params *params) override
        {
            if (!transition.Calculate(active))
                return;

            for (int i = 0; i < width; i++)
            {
                // this time we pass a positional argument that has the value x+y. We add 2 to make it
                // range between 0 and 4. As width argument we pass 4.
                // This will result in a diagonal transition
                pixels[i] = params->getSecondaryColour() * transition.getValue(map->x(i) + map->y(i) + 2, 4);
            }
        }
    };

};


