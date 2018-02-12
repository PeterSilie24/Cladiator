/* Cladiator - Class Diagram Editor
 * Copyright (C) 2017 Phil Badura
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "reg-exp.hpp"

#define REG_EXP_DEFAULT "([^ ^\\^\\!\\\"\\%\\&\\/\\(\\)\\=\\?\\+\\*\\~\\@\\#\\<\\>\\|\\,\\;\\.\\:\\-]{0,})"

#define REG_EXP_TYPE_FUNC(REG_EXP) "(" REG_EXP_DEFAULT "(<" REG_EXP ">)?)"

#define REG_EXP_TYPE REG_EXP_TYPE_FUNC(REG_EXP_TYPE_FUNC(REG_EXP_TYPE_FUNC(REG_EXP_TYPE_FUNC(REG_EXP_TYPE_FUNC(REG_EXP_TYPE_FUNC(REG_EXP_TYPE_FUNC(REG_EXP_TYPE_FUNC(REG_EXP_TYPE_FUNC(REG_EXP_TYPE_FUNC(REG_EXP_DEFAULT))))))))))

#define REG_EXP_VALUE "((" REG_EXP_DEFAULT "|(\"([^\"])*\")|('([^'])*')){0,})"

#define REG_EXP_NUMBERS "([0-9]{1,})"

#define REG_EXP_MULTIPLICITY "((" REG_EXP_NUMBERS "|[\\*nm])|((" REG_EXP_NUMBERS "|[\\*nm])\\.\\.(" REG_EXP_NUMBERS "|[\\*nm])))"

const QString RegExp::Default(REG_EXP_DEFAULT);

const QString RegExp::Type(REG_EXP_TYPE);

const QString RegExp::Value(REG_EXP_VALUE);

const QString RegExp::Multiplicity(REG_EXP_MULTIPLICITY);

RegExp::RegExp()
{
	if (!this->weakValidators)
	{
		this->strongValidators = QSharedPointer<QMap<QString, QSharedPointer<QRegExpValidator>>>(new QMap<QString, QSharedPointer<QRegExpValidator>>());

		this->weakValidators = this->strongValidators.toWeakRef();
	}
}

QRegExpValidator* RegExp::getValidator(const QString& regExp)
{
	QSharedPointer<QMap<QString, QSharedPointer<QRegExpValidator>>> validators = weakValidators.toStrongRef();
	
	if (validators)
	{
		if (validators->contains(regExp))
		{
			return (*validators)[regExp].data();
		}

		QSharedPointer<QRegExpValidator> validator(new QRegExpValidator(QRegExp(regExp)));

		validators->insert(regExp, validator);

		return validator.data();
	}

	return nullptr;
}

QString RegExp::validate(const QString& string, const QString& regExp)
{
	QString stringBuff = string;

	int pos = 0;

	QRegExpValidator* validator = getValidator(regExp);

	if (validator)
	{
		if (validator->validate(stringBuff, pos) == QValidator::State::Acceptable)
		{
			return string;
		}
	}

	return "";
}

QWeakPointer<QMap<QString, QSharedPointer<QRegExpValidator>>> RegExp::weakValidators;
