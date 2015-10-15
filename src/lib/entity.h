/*
 * Copyright (C) 2014 Lucien XU <sfietkonstantin@free.fr>
 *
 * You may use this file under the terms of the BSD license as follows:
 *
 * "Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 *   * The names of its contributors may not be used to endorse or promote
 *     products derived from this software without specific prior written
 *     permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
 */

#ifndef ENTITY_H
#define ENTITY_H

#include <QtCore/QJsonObject>
#include <memory>

class EntityVisitor;
/**
 * @brief An entity
 *
 * This class represents an entity from Twitter API.
 *
 * An entity is described with a Type. Each type
 * is available as a subclass of this class.
 *
 * See https://dev.twitter.com/overview/api/entities-in-twitter-objects.
 */
class Entity
{
public:
    using Ptr = std::shared_ptr<Entity>;
    using List = std::vector<Ptr>;
    virtual ~Entity() {}
    /**
     * @brief If the entity instance is valid
     * @return if the entity instance is valid.
     */
    virtual bool isValid() const = 0;
    /**
     * @brief Text captured by the entity
     * @return text captured by the entity.
     */
    virtual QString text() const = 0;
    virtual void accept(EntityVisitor &visitor) const = 0;
    /**
     * @brief Creates a list of Entity from a JSON object
     *
     * This factory method parses the input JSON objects
     * that is retrieved from Twitter to create a list of
     * of Entity subclasses that corresponds to the parsed entity.
     *
     * This method parses entities and extended_entities.
     *
     * @param json JSON object to parse.
     * @param extendedJson JSON object to parse (extended_entities).
     * @return an list of entities.
     */
    static List create(const QJsonObject &json, const QJsonObject &extendedJson = QJsonObject());
};

#endif // ENTITY_H
