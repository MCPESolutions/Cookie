#pragma once
#include "../Utils/Maths.h"
#include <cmath>
#include <algorithm>

/**
 * AnimationUtil - �ṩ˿���Ķ�����ֵ����
 * ֧�� Vec2, Vec3 �ͻ����������͵�ƽ������
 */
class AnimationUtil {
public:
    // Vec2 ��������
    /**
     * �� Vec2 ����˿����ֵ����
     * @param current ��ǰֵ���ᱻ�޸ģ�
     * @param target Ŀ��ֵ
     * @param speed �����ٶ� (���鷶Χ: 1.0f - 20.0f)
     * @param deltaTime ֡ʱ���
     * @param threshold ֹͣ��ֵ�����ӽ�Ŀ��ֵʱֱ����ΪĿ��ֵ
     * @return �Ƿ��ѵ���Ŀ��ֵ
     */
    static bool lerpVec2(Vec2<float>& current, const Vec2<float>& target, float speed, float deltaTime, float threshold = 0.01f);

    // Vec3 ��������
    /**
     * �� Vec3 ����˿����ֵ����
     * @param current ��ǰֵ���ᱻ�޸ģ�
     * @param target Ŀ��ֵ
     * @param speed �����ٶ� (���鷶Χ: 1.0f - 20.0f)
     * @param deltaTime ֡ʱ���
     * @param threshold ֹͣ��ֵ�����ӽ�Ŀ��ֵʱֱ����ΪĿ��ֵ
     * @return �Ƿ��ѵ���Ŀ��ֵ
     */
    static bool lerpVec3(Vec3<float>& current, const Vec3<float>& target, float speed, float deltaTime, float threshold = 0.01f);

    // float ��������
    /**
     * �� float ����˿����ֵ����
     * @param current ��ǰֵ���ᱻ�޸ģ�
     * @param target Ŀ��ֵ
     * @param speed �����ٶ� (���鷶Χ: 1.0f - 20.0f)
     * @param deltaTime ֡ʱ���
     * @param threshold ֹͣ��ֵ�����ӽ�Ŀ��ֵʱֱ����ΪĿ��ֵ
     * @return �Ƿ��ѵ���Ŀ��ֵ
     */
    static bool lerpFloat(float& current, float target, float speed, float deltaTime, float threshold = 0.01f);

    // ������������ (��ƽ���Ķ�������)
    /**
     * ������Ч���� Vec2 ���� (EaseOut)
     * @param current ��ǰֵ���ᱻ�޸ģ�
     * @param target Ŀ��ֵ
     * @param speed �����ٶ� (���鷶Χ: 5.0f - 30.0f)
     * @param deltaTime ֡ʱ���
     * @param threshold ֹͣ��ֵ
     * @return �Ƿ��ѵ���Ŀ��ֵ
     */
    static bool easeOutVec2(Vec2<float>& current, const Vec2<float>& target, float speed, float deltaTime, float threshold = 0.01f);

    /**
     * ������Ч���� Vec3 ���� (EaseOut)
     * @param current ��ǰֵ���ᱻ�޸ģ�
     * @param target Ŀ��ֵ
     * @param speed �����ٶ� (���鷶Χ: 5.0f - 30.0f)
     * @param deltaTime ֡ʱ���
     * @param threshold ֹͣ��ֵ
     * @return �Ƿ��ѵ���Ŀ��ֵ
     */
    static bool easeOutVec3(Vec3<float>& current, const Vec3<float>& target, float speed, float deltaTime, float threshold = 0.01f);

    /**
     * ������Ч���� float ���� (EaseOut)
     * @param current ��ǰֵ���ᱻ�޸ģ�
     * @param target Ŀ��ֵ
     * @param speed �����ٶ� (���鷶Χ: 5.0f - 30.0f)
     * @param deltaTime ֡ʱ���
     * @param threshold ֹͣ��ֵ
     * @return �Ƿ��ѵ���Ŀ��ֵ
     */
    static bool easeOutFloat(float& current, float target, float speed, float deltaTime, float threshold = 0.01f);

    // ���Զ������� (������Ч��)
    /**
     * ���Զ���Ч���� Vec2 ����
     * @param current ��ǰֵ���ᱻ�޸ģ�
     * @param target Ŀ��ֵ
     * @param speed �����ٶ� (���鷶Χ: 8.0f - 25.0f)
     * @param deltaTime ֡ʱ���
     * @param elasticity ����ǿ�� (���鷶Χ: 0.1f - 0.5f)
     * @param threshold ֹͣ��ֵ
     * @return �Ƿ��ѵ���Ŀ��ֵ
     */
    static bool elasticVec2(Vec2<float>& current, const Vec2<float>& target, float speed, float deltaTime, float elasticity = 0.1f, float threshold = 0.01f);

    /**
     * ���Զ���Ч���� float ����
     * @param current ��ǰֵ���ᱻ�޸ģ�
     * @param target Ŀ��ֵ
     * @param speed �����ٶ� (���鷶Χ: 8.0f - 25.0f)
     * @param deltaTime ֡ʱ���
     * @param elasticity ����ǿ�� (���鷶Χ: 0.1f - 0.5f)
     * @param threshold ֹͣ��ֵ
     * @return �Ƿ��ѵ���Ŀ��ֵ
     */
    static bool elasticFloat(float& current, float target, float speed, float deltaTime, float elasticity = 0.1f, float threshold = 0.01f);

    // ������������Ч����
    /**
     * ���㲨��ƫ�������������ֵĲ�����Ч��
     * @param time ��ǰʱ��
     * @param frequency ���˵�Ƶ��
     * @param amplitude ���˵ķ���
     * @param phase ��λƫ��
     */
    static float getWaveOffset(float time, float frequency, float amplitude, float phase = 0.0f);

    /**
     * ����ɨ��Ч���Ľ��ȣ��������ֵ�ɨ����Ч��
     * @param time ��ǰʱ��
     * @param duration ɨ��Ч������ʱ��
     * @param delay �ӳ�ʱ��
     */
    static float getSweepProgress(float time, float duration, float delay = 0.0f);

    /**
     * ��������Ч�����������ֵ���˸��Ч��
     * @param time ��ǰʱ��
     * @param speed ��˸�ٶ�
     */
    static float getShimmerEffect(float time, float speed = 2.0f);

    /**
     * ���㷢������Ч�����������ֵ�������Ч��
     * @param time ��ǰʱ��
     * @param frequency ����Ƶ��
     */
    static float getGlowPulse(float time, float frequency = 3.0f);

    /**
     * ���㵯��Ч�����������ֵĵ�����Ч��
     * @param progress ��������
     * @param bounceHeight �����߶�
     */
    static float getBounceEffect(float progress, float bounceHeight = 0.3f);

    /**
     * ���㵯��Ч�����������ֵĵ�����Ч��
     * @param progress ��������
     * @param elasticity ����ϵ��
     */
    static float getElasticEffect(float progress, float elasticity = 0.5f);

    /**
     * ����ˮ����Ч�����������ֵĲ�����Ч��
     * @param time ��ǰʱ��
     * @param distance ������ɢ�ľ���
     * @param speed ���ƴ����ٶ�
     */
    static float getRippleEffect(float time, float distance, float speed = 2.0f);

    // �߼���������
    /**
     * ���η��������������ڸ߼�������ֵ��
     */
    static float easeInOutCubic(float t);

    /**
     * �Ĵη��������������ڸ߼�������ֵ��
     */
    static float easeInOutQuart(float t);

    /**
     * �������߻������������ڸ߼�������ֵ��
     */
    static float easeInOutSine(float t);

    /**
     * �����������������ڸ߼�������ֵ��
     */
    static float easeOutBounce(float t);

    /**
     * ���Ի������������ڸ߼�������ֵ��
     */
    static float easeInElastic(float t);
    static float easeOutElastic(float t);

    // ���ߺ���
    /**
     * �������� Vec2 ֮��ľ���
     */
    static float distance(const Vec2<float>& a, const Vec2<float>& b);

    /**
     * �������� Vec3 ֮��ľ���
     */
    static float distance(const Vec3<float>& a, const Vec3<float>& b);

    /**
     * ����ֵ��ָ����Χ��
     */
    static float clamp(float value, float min, float max);

    /**
     * �������Բ�ֵ����
     */
    static float lerp(float a, float b, float t);

    /**
     * ƽ����ֵ����������ƽ�����������̣�
     */
    static float smoothstep(float edge0, float edge1, float x);
};
