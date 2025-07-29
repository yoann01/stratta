
/*Qt headers*/
#include <QApplication>
#include <QIcon>
#include <QGuiApplication>
#include <QCoreApplication>
#include <QDir>
#include <QDebug>



#include <oclero/qlementine/style/QlementineStyle.hpp>
#include <oclero/qlementine/style/ThemeManager.hpp>
#include <oclero/qlementine/icons/QlementineIcons.hpp>


#include "mainwindow/mainwindow.h"

// USD includes for plugin setup
#include <pxr/base/plug/registry.h>
#include <pxr/base/tf/getenv.h>

PXR_NAMESPACE_USING_DIRECTIVE

#define USE_CUSTOM_STYLE 1


void setupUsdPluginPaths() {
    // Get the application directory
    QString appDir = QCoreApplication::applicationDirPath();
    qDebug() << "Application directory:" << appDir;


    // Set up potential plugin paths
    QStringList pluginPaths;
    pluginPaths << appDir + "/lib/usd";
    pluginPaths << appDir + "/plugin";
    pluginPaths << appDir + "/bin/usd";

    // Join paths with the system separator (';' on Windows, ':' on Unix)
    QString pathSeparator = ";"; // Windows
#ifdef Q_OS_UNIX
    pathSeparator = ":";
#endif

    QString pluginPathValue = pluginPaths.join(pathSeparator);

    // Set the environment variable
    qputenv("PXR_PLUGINPATH_NAME", pluginPathValue.toLocal8Bit());

    // Enable debug output to see what plugins are being loaded
    qputenv("TF_DEBUG", "PLUG_INFO_SEARCH,PLUG_LOAD");

    qDebug() << "USD Plugin paths set to:" << pluginPathValue;

    // Force plugin registry to rescan
    PlugRegistry& registry = PlugRegistry::GetInstance();
    registry.RegisterPlugins(pluginPaths.first().toStdString());

    qDebug() << "USD Plugin paths set to:" << pluginPathValue;
}


int main(int argc, char* argv[])
{

    QApplication app(argc, argv);

    qDebug() << "=== APPLICATION STARTUP ===";
    setupUsdPluginPaths();

    QGuiApplication::setApplicationDisplayName("Stratta");
    QCoreApplication::setApplicationName(QStringLiteral("Storyforge"));
    QCoreApplication::setOrganizationName(QStringLiteral("---"));
    QCoreApplication::setOrganizationDomain(QStringLiteral("---.com"));
    QCoreApplication::setApplicationVersion("1.0.0");
    QApplication::setWindowIcon(QIcon(QStringLiteral(":/showcase/qlementine_icon.ico")));



#if USE_CUSTOM_STYLE
    // Custom QStyle.
    auto* style = new oclero::qlementine::QlementineStyle(&app);
    style->setAnimationsEnabled(true);
    style->setAutoIconColor(oclero::qlementine::AutoIconColor::TextColor);
    style->setIconPathGetter(oclero::qlementine::icons::fromFreeDesktop);
    app.setStyle(style);

    // Custom icon theme.
    oclero::qlementine::icons::initializeIconTheme();
    QIcon::setThemeName("qlementine");

    // Theme manager.
    auto* themeManager = new oclero::qlementine::ThemeManager(style);
    themeManager->loadDirectory(":/showcase/themes");

    // Define theme on QStyle.
    themeManager->setCurrentTheme("Dark-theme");

    auto window = std::make_unique<MainWindow>(themeManager);
#else
    auto window = std::make_unique<MainWindow>(nullptr);
#endif

    window->show();

    return app.exec();
}