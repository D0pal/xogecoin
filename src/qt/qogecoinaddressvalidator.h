// Copyright (c) 2011-2020 The Bitcoin and Qogecoin Core Authors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef XOGECOIN_QT_XOGECOINADDRESSVALIDATOR_H
#define XOGECOIN_QT_XOGECOINADDRESSVALIDATOR_H

#include <QValidator>

/** Base58 entry widget validator, checks for valid characters and
 * removes some whitespace.
 */
class QogecoinAddressEntryValidator : public QValidator
{
    Q_OBJECT

public:
    explicit QogecoinAddressEntryValidator(QObject *parent);

    State validate(QString &input, int &pos) const override;
};

/** Qogecoin address widget validator, checks for a valid xogecoin address.
 */
class QogecoinAddressCheckValidator : public QValidator
{
    Q_OBJECT

public:
    explicit QogecoinAddressCheckValidator(QObject *parent);

    State validate(QString &input, int &pos) const override;
};

#endif // XOGECOIN_QT_XOGECOINADDRESSVALIDATOR_H
