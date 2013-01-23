#include <Wt/WServer>
#include <Wt/WResource>
#include <Wt/Http/Request>
#include <Wt/Http/Response>
#include <Wt/WEnvironment>
#include <Wt/WApplication>

#include "GlobalIncludeFile.h"
#include "tools/SessionPool.h"
#include <mutex>



SessionPool* SessionPool::instance = 0;
std::string SessionPool::credentials = "";
std::mutex SessionPool::mutex;

//---------------------------------------------------------------------------

Wt::WApplication *createEchoesHomeApplication(const Wt::WEnvironment& env) 
{
    // On instancie la classe EchoesHome qui permet d'afficher le site.
    Wt::WApplication *app = new Wt::WApplication(env);
    
    app->setTitle("ECHOES Alert");
    
    app->messageResourceBundle().use("global",false);
    
    app->useStyleSheet(Wt::WApplication::resourcesUrl() + "form.css");
    app->useStyleSheet(Wt::WApplication::resourcesUrl() + "form-alert.css");
    app->useStyleSheet(Wt::WApplication::resourcesUrl() + "form-user.css");
    app->useStyleSheet(Wt::WApplication::resourcesUrl() + "form-probe.css");
    app->useStyleSheet(Wt::WApplication::resourcesUrl() + "alert.css");
    
    app->setCssTheme("polished");
  

    new EchoesHome(app->root());

    return app;
    
}

/**
Point d'entrée du programme.
@param argc : TBC
@param argv : TBC
*/
int main(int argc, char **argv)
{
    try
    {
        // On passe le premier paramètre d'entrée au serveur
        Wt::WServer server(argv[0]);
        // On définit la configuration du serveur en lui passant les paramètres d'entrée et son fichier de configuration
        server.setServerConfiguration(argc, argv);
        // On fixe le point d'entrée du programme (type de point d'entée, méthode à appeler, uri, chemin favicon)
        
        server.addEntryPoint(Wt::Application, createEchoesHomeApplication,"", "/favicon.ico");
        
        Session::configureAuth();
        
        // démarrage du serveur en arrière plan
        if (server.start())
        {
            // méthode qui bloque le thread courant en attendant le signal d'exctinction
            Wt::WServer::waitForShutdown();
            server.stop();
        }
    }
    catch (Wt::WServer::Exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
    catch (std::exception &e)
    {
        std::cerr << "exception: " << e.what() << std::endl;
    }
}
