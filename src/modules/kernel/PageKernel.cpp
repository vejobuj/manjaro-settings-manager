/*
 *  Manjaro Settings Manager
 *  Ramon Buldó <ramon@manjaro.org>
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

#include "PageKernel.h"
#include "ui_PageKernel.h"

#include "KernelListViewDelegate.h"

#include <KAboutData>
#include <KAuth>
#include <KAuthAction>

#include <QtCore/QProcess>
#include <QtWidgets/QMessageBox>

#include <QDebug>

#include <KPluginFactory>
K_PLUGIN_FACTORY(MsmKernelFactory,
                 registerPlugin<PageKernel>("msm_kernel");)

PageKernel::PageKernel(QWidget *parent, const QVariantList &args) :
    KCModule(parent, args),
    ui(new Ui::PageKernel),
    kernelModel(new KernelModel),
    kernelInfoDialog(new KernelInfoDialog)
{
    KAboutData *aboutData = new KAboutData("msm_kernel",
                                           i18nc("@title", "Kernel"),
                                           PROJECT_VERSION,
                                           QStringLiteral(""),
                                           KAboutLicense::LicenseKey::GPL_V3,
                                           i18nc("@info:credit", "Copyright 2014 Ramon Buldó"));

    aboutData->addAuthor(i18nc("@info:credit", "Ramon Buldó"),
                         i18nc("@info:credit", "Author"),
                         QStringLiteral("ramon@manjaro.org"));

    setAboutData(aboutData);  
  
    ui->setupUi(this);

    KernelSortFilterProxyModel *proxyKernelModel = new KernelSortFilterProxyModel(this);
    proxyKernelModel->setSourceModel(kernelModel);
    proxyKernelModel->setSortRole(KernelModel::VersionRole);
    proxyKernelModel->sort(0, Qt::DescendingOrder);
    ui->kernelListView->setModel(proxyKernelModel);

    KernelListViewDelegate *kernelListViewDelegate = new KernelListViewDelegate;
    ui->kernelListView->setItemDelegate(kernelListViewDelegate);
    connect(kernelListViewDelegate, &KernelListViewDelegate::installButtonClicked,
            this, &PageKernel::installButtonClicked);
    connect(kernelListViewDelegate, &KernelListViewDelegate::infoButtonClicked,
            this, &PageKernel::infoButtonClicked);
}

PageKernel::~PageKernel()
{
    delete ui;
    delete kernelModel;
}

void PageKernel::load()
{
    kernelModel->update();
}

void PageKernel::save()
{
}

void PageKernel::defaults()
{
    kernelModel->update();
}

void PageKernel::installButtonClicked(const QModelIndex &index)
{
    bool isInstalled = qvariant_cast<bool>(index.data(KernelModel::IsInstalledRole));
    if (isInstalled) {
        removeKernel(index);
    } else {
        installKernel(index);
    }
}

void PageKernel::installKernel(const QModelIndex &index)
{
    QStringList modules = qvariant_cast<QStringList>(index.data(KernelModel::InstalledModulesRole));
    QString kernel = qvariant_cast<QString>(index.data(KernelModel::PackageRole));
    QString version = qvariant_cast<QString>(index.data(KernelModel::VersionRole));
    QStringList packageList;
    packageList << kernel << modules;

    QString title = QString(tr("Install Linux %1")).arg(version);
    QString message = QString(tr("Do you really want to install this kernel?\n"));
    QString information = QString(tr("This will install the following packages:\n"));
    for (QString p : packageList) {        
        information.append(p);
        information.append("\n");
    }

    QMessageBox messageBox;
    messageBox.setIcon(QMessageBox::Question);
    messageBox.setWindowTitle(title);
    messageBox.setText(message);
    messageBox.setDetailedText(information);
    messageBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    messageBox.setDefaultButton(QMessageBox::No);

    int reply = messageBox.exec();
    if (reply == QMessageBox::Yes) {
        //TODO: Properly implement installing kernels
        QStringList arguments;
        arguments << "--noconfirm" << "-S" << packageList;
        QVariantMap args;
        args["arguments"] = arguments;
        KAuth::Action installAction(QLatin1String("org.manjaro.msm.kernel.install"));
        installAction.setHelperId(QLatin1String("org.manjaro.msm.kernel"));
        installAction.setArguments(args);
        KAuth::ExecuteJob *job = installAction.execute();
        connect(job, &KAuth::ExecuteJob::newData,
                [=] (const QVariantMap &data)
        {
            qDebug() << data;
        });
        if (job->exec()) {
            qDebug() << "Job Succesfull";
        } else {
            qDebug() << "Job Failed";
        }
        kernelModel->update();
    }
}

void PageKernel::removeKernel(const QModelIndex &index)
{
    QStringList modules = qvariant_cast<QStringList>(index.data(KernelModel::InstalledModulesRole));
    QString kernel = qvariant_cast<QString>(index.data(KernelModel::PackageRole));
    QString version = qvariant_cast<QString>(index.data(KernelModel::VersionRole));
    QStringList packageList;
    packageList << kernel << modules;

    QString title = QString(tr("Remove Linux %1")).arg(version);
    QString message = QString(tr("Do you really want to remove this kernel?"));
    QString information = QString(tr("This will remove the following packages:\n"));
    for (QString p : packageList) {        
        information.append(p);
        information.append("\n");
    }

    QMessageBox messageBox;
    messageBox.setIcon(QMessageBox::Question);
    messageBox.setWindowTitle(title);
    messageBox.setText(message);
    messageBox.setDetailedText(information);
    messageBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    messageBox.setDefaultButton(QMessageBox::No);

    int reply = messageBox.exec();
    if (reply == QMessageBox::Yes) {
        //TODO: Properly implement removing kernels
        QStringList arguments;
        arguments << "--noconfirm" << "-R" << packageList;
        QVariantMap args;
        args["arguments"] = arguments;
        KAuth::Action installAction(QLatin1String("org.manjaro.msm.kernel.remove"));
        installAction.setHelperId(QLatin1String("org.manjaro.msm.kernel"));
        installAction.setArguments(args);
        KAuth::ExecuteJob *job = installAction.execute();
        connect(job, &KAuth::ExecuteJob::newData,
                [=] (const QVariantMap &data)
        {
            qDebug() << data;
        });
        if (job->exec()) {
            qDebug() << "Job Succesful";
        } else {
            qDebug() << "Job Failed";
        }
        kernelModel->update();
    }
}

void PageKernel::infoButtonClicked(const QModelIndex &index)
{
    QString package = qvariant_cast<QString>(index.data(KernelModel::PackageRole));
    QString majorVersion = qvariant_cast<QString>(index.data(KernelModel::MajorVersionRole));
    QString minorVersion = qvariant_cast<QString>(index.data(KernelModel::MinorVersionRole));
    QString title = QString(tr("Linux %1.%2 changelog")).arg(majorVersion, minorVersion);
    kernelInfoDialog->setWindowTitle(title);
    kernelInfoDialog->setPackage(package);
    kernelInfoDialog->exec();
}

#include "PageKernel.moc"
