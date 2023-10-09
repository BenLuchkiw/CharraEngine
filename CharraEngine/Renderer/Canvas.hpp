#pragma once





namespace Charra
{
    /*
        This class should manage all drawing operations
        
        Users of this class will be users, and window manager

        Needs abiliity to record to command buffer, with expectation that
        recording began before calling function and will close after,
        this is because renderer::draw will iterate and record each canvas to the same buffer

        Does not need to know anything about window other than the dimensions,
        the best way to do this would probably be a simple change dimension function,
        if other parts of the program need this information they will need to ask canvas
        for dimensions, callbacks are unnecessary at this time

        Needs to make, store, and manage all vertex and index information in vertex buffers, 
        needs to be able to submit buffers on request for transfers

        Might want the ability to optimize buffers, future consideration, 
        would be a big task




    */
    class Canvas
    {
    public:

    private: // Methods

    private: // Members


    };



    /*
    
    This class will manage the relationship between windows and canvases
    and should be moved from this file into either window.hpp or its own file

    When windows get destroyed they will need to report to this class so the canvasses can be 
    dissasociated and renderer::draw() wont create errors
    
    */
    class WindowCanvasManager
    {
    public:

    private: // Methods

    private: // Members
    };
}