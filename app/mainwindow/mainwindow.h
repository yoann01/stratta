#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

// Qt headers.
#include <QWidget>

#include <memory>

// forward declartation to avoid include
namespace oclero::qlementine {
    class QlementineStyle;
    class ThemeManager;
}

// Use the correct namespace alias
using ThemeManager = oclero::qlementine::ThemeManager;
using QlementineStyle = oclero::qlementine::QlementineStyle;


class MainWindow : public QWidget
{
    Q_OBJECT
public:
    MainWindow(ThemeManager* themeManager = nullptr, QWidget* parent = nullptr);
    ~MainWindow();

private:
    struct Impl;
    std::unique_ptr<Impl> _impl{};
};





#endif // MAINWINDOW_HPP