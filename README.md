# Favro to Unreal Engine Link
A simple Favro integration for Unreal Engine to quick-check boards and post into a dedicated Bug Report collection.

This Unreal Plugin is an on-going personal endeavour to practice Slate, Editor Customisation, System Architecture, and HTTP requests in Unreal. As such, please refrain from pull requests and rather give me a piece of your mind on [LinkedIn](https://www.linkedin.com/in/marianhev) or [Discord](https://discord.com/users/novembrinewaltz) so I can improve.

![dashboardview.png](img%2Fdashboardview.png)

It is far from functional at the moment. As a matter of fact, I haven't even implemented the darn thing I made it for yet - dedicated Bug Report from inside the Engine.

The goal is more of an excuse for the journey than anything. However, some greater minds might get inspired by it and contribute something meaningful on their own. Especially if, like me, you are a bit weary of colleagues saying they haven't checked Favro in a minute.
## Getting Started
### Configuration
You can call the plug-in window from `Window > Favro Link` in Engine.
There, you can fill `Username` and `Api Token`. 

![auth.png](img%2Fauth.png)

You can [Generate an Api Token](https://learn.microsoft.com/en-us/viva/goals/favro-generating-an-api-token) from your Favro profile. You can also use your password - but that's up to you.

All config keys are stored in `FavroLink.ini` in your project `Saved\Config\WindowsEditor` directory. This is usually left unsourced by the sane of mind.

## Usage
That's it, that's all you can do by now. Just browse the collections, and board, and cards. And sometimes it might crash, or Slate might bug behind.

![widgetstest.gif](img%2Fwidgetstest.gif)

## To-dos
- [ ] Finish Favro Types (Tags, Tasks, Comments, Etc...)
- [ ] Finish GET Methods
- [ ] Better Request Flow
- [ ] Better Runtime Caching (Messenger & Model)
- [ ] Pop-up Favro Task View
- [ ] Pop-up Bug Report
- [ ] Implement a couple POST Methods
- [ ] \(Optional) Webhooks

## Resources Used
- Icons from [Lucide](https://github.com/lucide-icons/lucide)
- READMEs from [Bravo](https://github.com/bscotch/favro-sdk) & [Favro PHP Api](https://github.com/seregazhuk/php-favro-api)
- Lurking on [BenUI](https://benui.ca/) Discord