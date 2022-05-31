#pragma once

#include "chipem.hpp"
#include "SFML/Graphics.hpp"

#include <thread>
#include <chrono>

namespace chipe
{
    sf::View getLetterboxView(sf::View view, int windowWidth, int windowHeight) 
    {
        // Compares the aspect ratio of the window to the aspect ratio of the view,
        // and sets the view's viewport accordingly in order to archieve a letterbox effect.
        // A new view (with a new viewport set) is returned.

        float windowRatio = windowWidth / (float) windowHeight;
        float viewRatio = view.getSize().x / (float) view.getSize().y;
        float sizeX = 1;
        float sizeY = 1;
        float posX = 0;
        float posY = 0;

        bool horizontalSpacing = true;
        if (windowRatio < viewRatio)
            horizontalSpacing = false;

        // If horizontalSpacing is true, the black bars will appear on the left and right side.
        // Otherwise, the black bars will appear on the top and bottom.

        if (horizontalSpacing) {
            sizeX = viewRatio / windowRatio;
            posX = (1 - sizeX) / 2.f;
        }

        else {
            sizeY = windowRatio / viewRatio;
            posY = (1 - sizeY) / 2.f;
        }

        view.setViewport( sf::FloatRect(posX, posY, sizeX, sizeY) );

        return view;
    }

    unsigned char Emulator::Running()
    {
        return this->run;
    }

    void Emulator::mapKeyboard(char key, BYTE value)
    {
        switch(key)
        {
            case '1': this->cpu->setKey(0x1,value); break;
            case '2': this->cpu->setKey(0x2,value); break;
            case '3': this->cpu->setKey(0x3,value); break;
            case '4': this->cpu->setKey(0xC,value); break;
            case 'Q': this->cpu->setKey(0x4,value); break;
            case 'W': this->cpu->setKey(0x5,value); break;
            case 'E': this->cpu->setKey(0x6,value); break;
            case 'R': this->cpu->setKey(0xD,value); break;
            case 'A': this->cpu->setKey(0x7,value); break;
            case 'S': this->cpu->setKey(0x8,value); break;
            case 'D': this->cpu->setKey(0x9,value); break;
            case 'F': this->cpu->setKey(0xE,value); break;
            case 'Z': this->cpu->setKey(0xA,value); break;
            case 'X': this->cpu->setKey(0x0,value); break;
            case 'C': this->cpu->setKey(0xB,value); break;
            case 'V': this->cpu->setKey(0xF,value); break;
            
            default: break;
        }
    }

    void Emulator::handleInputs(sf::Event event)
    {
        bool press=0;
        if(event.type==sf::Event::KeyPressed) press=1;

        auto key=event.key.code;
        switch(key)
        {
            case sf::Keyboard::Num1: mapKeyboard('1',(BYTE)press); break;
            case sf::Keyboard::Num2: mapKeyboard('2',(BYTE)press); break;
            case sf::Keyboard::Num3: mapKeyboard('3',(BYTE)press); break;
            case sf::Keyboard::Num4: mapKeyboard('4',(BYTE)press); break;
            case sf::Keyboard::Q: mapKeyboard('Q',(BYTE)press); break;
            case sf::Keyboard::W: mapKeyboard('W',(BYTE)press); break;
            case sf::Keyboard::E: mapKeyboard('E',(BYTE)press); break;
            case sf::Keyboard::R: mapKeyboard('R',(BYTE)press); break;
            case sf::Keyboard::A: mapKeyboard('A',(BYTE)press); break;
            case sf::Keyboard::S: mapKeyboard('S',(BYTE)press); break;
            case sf::Keyboard::D: mapKeyboard('D',(BYTE)press); break;
            case sf::Keyboard::F: mapKeyboard('F',(BYTE)press); break;
            case sf::Keyboard::Z: mapKeyboard('Z',(BYTE)press); break;
            case sf::Keyboard::X: mapKeyboard('X',(BYTE)press); break;
            case sf::Keyboard::C: mapKeyboard('C',(BYTE)press); break;
            case sf::Keyboard::V: mapKeyboard('V',(BYTE)press); break;

            default: break;
        }
    }

    void Emulator::setOPS(unsigned long long int OPS)
    {
        this->MaxOPS=OPS;
    }

    void Emulator::MakeMatrix(sf::Color color)
    {
        float xsize=this->resX/32, ysize=this->resY/64;
        for(int i=0; i<64; ++i)
        {
            for(int j=0; j<32; ++j)
            {
                sf::RectangleShape rect(sf::Vector2f(xsize,ysize));
                sf::Vector2f position(xsize*j,ysize*i);
                rect.setPosition(position);
                rect.setFillColor(color);
                this->matrix[i][j]=rect;
            }
        }
    }

    void Emulator::MakeMatrix()
    {
        float xsize=this->resX/32, ysize=this->resY/64;
        for(int i=0; i<64; ++i)
        {
            for(int j=0; j<32; ++j)
            {
                sf::RectangleShape rect(sf::Vector2f(xsize,ysize));
                sf::Vector2f position(xsize*j,ysize*i);
                sf::Color color;
                if(this->cpu->ScreenData[i][j]==0)
                    color=sf::Color::Black;
                else
                    color=sf::Color::White;
                
                rect.setPosition(position);
                rect.setFillColor(color);
                this->matrix[i][j]=rect;
            }
        }
    }

    void Emulator::InitCPU()
    {
        this->run=1;
        this->cpu->Reset();
    }

    void Emulator::InitRenderer()
    {
        this->resX=640;
        this->resY=640;

        this->MakeMatrix(sf::Color::Black);

        this->view.setSize(resX,resY);
        this->view.setCenter(view.getSize().x / 2, view.getSize().y / 2);
        this->view=getLetterboxView(view, resX, resY);
        this->view.setRotation(-90);
        auto size=this->view.getSize();
        size.x*=-1;
        this->view.setSize(size);
    }

    void Emulator::Render()
    {
        if(this->RScreen.isOpen())
        {
            // this->cpu->clearKeys();
            sf::Event event;
            while (this->RScreen.pollEvent(event)) 
            {
                switch(event.type)
                {
                    case sf::Event::Closed : this->RScreen.close(); this->run=0; break;
                    case sf::Event::Resized: view = getLetterboxView( view, event.size.width, event.size.height );
                    case sf::Event::KeyPressed: handleInputs(event); break;
                    case sf::Event::KeyReleased: handleInputs(event); break;

                    default: break;
                }
            }

            this->RScreen.clear(sf::Color(55,50,84,255));
            
            // Translate 
            MakeMatrix();

            // Draw the grid Matrix
            for(auto &i : this->matrix)
            {
                for(auto &j: i)
                {
                    this->RScreen.draw(j);
                }
            }

            this->RScreen.setView(view);
            this->RScreen.display();
        }
    }

    void Emulator::Cycle(){
        for(int i=0; i<this->MaxOPS; ++i)
        {
            WORD current; // current opcode

            current=this->cpu->getNextOpcode();

            this->cpu->DecodeOpcode(current);
            this->cpu->Timer();

            this->Render();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1000/MaxOPS));
    }
}