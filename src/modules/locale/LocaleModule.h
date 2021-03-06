/*
 *  Manjaro Settings Manager
 *  Roland Singer <roland@manjaro.org>
 *  Ramon Buldo <ramon@manjaro.org>
 *
 *  Copyright (C) 2007 Free Software Foundation, Inc.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef LOCALEMODULE_H
#define LOCALEMODULE_H

#include "SelectLocalesDialog.h"
#include "LanguageListViewDelegate.h"
#include "EnabledLocalesModel.h"

#include <KCModule>

namespace Ui {
class LocaleModule;
}


class LocaleModule : public KCModule
{
    Q_OBJECT
    
public:
    /**
     * Constructor.
     *
     * @param parent Parent widget of the module
     * @param args Arguments for the module
     */
    explicit LocaleModule(QWidget *parent, const QVariantList &args = QVariantList());
    /**
     * Destructor.
     */
    ~LocaleModule();

    /**
     * Overloading the KCModule load() function.
     */
    void load();

    /**
     * Overloading the KCModule save() function.
     */
    void save();

    /**
     * Overloading the KCModule defaults() function.
     */
    void defaults();
    
private:
    Ui::LocaleModule *ui;
    EnabledLocalesModel *enabledLocalesModel_;
    LanguageListViewDelegate *languageListViewDelegate_;
    QString currentLocale_;
    bool isLocaleListModified_;
    bool isSystemLocalesModified_;
    QAction *setRegionAndFormatsAction_;
    QAction *setRegionAction_;
    QAction *setFormatsAction_;

protected slots:
    void addLocale();
    void removeLocale();
    void disableRemoveButton(const QModelIndex &current, const QModelIndex &previous);

};

#endif // LOCALEMODULE_H
