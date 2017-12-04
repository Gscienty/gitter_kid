import React, { Component } from 'react';
import { Layout, Input } from 'antd';

export default class BasePage extends Component {
    page() { return <div></div> }

    render() {
        return <Layout>
            <Layout.Header>
                <div
                    style={{ color: '#ffffff', fontSize: '27px', float: 'left', marginRight: '28px' }}
                >
                    Gitter Kid
                </div>

                <div
                    style={{ float: 'left', marginRight: '28px' }}
                >
                    <Input.Search
                        size="large"
                        style={{ width: '360px' }}
                        placeholder="Search Gitter Kid"
                        onSearch={
                            keyword => {
                                this.props.dispatch({
                                    type: 'repository/get',
                                    payload: { keyword }
                                });
                            }
                        }
                    />
                </div>
            </Layout.Header>
            <Layout.Content style={{ padding: '20px 50px' }}>
                <div style={{ padding: 24, background: '#ffffff' }}>
                    { this.page() }
                </div>
            </Layout.Content>
            <Layout.Footer style={{ textAlign: 'center' }}>
            </Layout.Footer>
        </Layout>
    }
}