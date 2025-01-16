#ifndef CHANNEL_HPP
#define CHANNEL_HPP

// #include <>

class Channel
{
    private:
        // std::<container> users;
    public:
        Channel();
        Channel(const Channel &other);
        Channel &operator=(const Channel &other);
        ~Channel();
}

#endif